// CopyrightHATO


#include "Character/AuraCharacterBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraHealthComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/AuraDebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

namespace AuraCharacterBaseConstants
{
	static const FName WeaponHandSocket(TEXT("WeaponHandSocket"));
}

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BurnDebuffComponent = CreateDefaultSubobject<UAuraDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	StunDebuffComponent = CreateDefaultSubobject<UAuraDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;

	HealthComponent = CreateDefaultSubobject<UAuraHealthComponent>("HealthComponent");

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), AuraCharacterBaseConstants::WeaponHandSocket);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	// SetUsingAbsoluteRotation: 부모 회전 무시 → 기존 Tick에서 매 프레임 SetWorldRotation(0) 강제하던 로직 대체 (Tick 제거)
	EffectAttachComponent->SetUsingAbsoluteRotation(true);

	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionNiagaraComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);

	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonNiagaraComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonNiagaraComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();

	// HealthComponent->StartDeath: 모든 클라이언트에서 호출되어야 외부 listener(DebuffNiagara 등)가 모든 머신에서 트리거됨.
	// StartDeath는 bDead idempotent라 서버에서 Die() 호출 시 중복 호출돼도 안전.
	HealthComponent->StartDeath(DeathImpulse);
}


UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

AActor* AAuraCharacterBase::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAuraCharacterBase::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// HealthComponent->StartDeath는 MulticastHandleDeath_Implementation 안에서 호출됨 (모든 클라이언트 트리거 보장).
	MulticastHandleDeath(DeathImpulse);
}

FOnDeathSignature& AAuraCharacterBase::GetOnDeathDelegate()
{
	// CombatInterface forward: 외부(AuraBeamSpell·DebuffNiagara)가 GetOnDeathDelegate()로 받는 델리게이트는
	// 실제로 HealthComponent.OnDeathStarted. 타입이 FOnDeathSignature로 통일돼 있어 호환.
	return HealthComponent->OnDeathStarted;
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	UpdateMovementSpeedFromDebuffs();
}

void AAuraCharacterBase::OnRep_Stunned()
{
	UpdateMovementSpeedFromDebuffs();
}

void AAuraCharacterBase::UpdateMovementSpeedFromDebuffs()
{
	const bool bMovementLocked = bIsStunned || bIsBeingShocked;
	GetCharacterMovement()->MaxWalkSpeed = bMovementLocked ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::BurnTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	bIsBurned = NewCount > 0;
}

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::OnRep_BeingShocked()
{
	UpdateMovementSpeedFromDebuffs();
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
		GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		for (const FTaggedMontage& TaggedMontage : AttackMontages)
		{
			if (TaggedMontage.SocketTag.MatchesTagExact(SocketTag))
			{
				return Weapon->GetSocketLocation(TaggedMontage.SocketName);
			}
		}
	}
	if (!SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
	{
		for (const FTaggedMontage& TaggedMontage : AttackMontages)
		{
			if (TaggedMontage.SocketTag.MatchesTagExact(SocketTag))
			{
				return GetMesh()->GetSocketLocation(TaggedMontage.SocketName);
			}
		}
	}
	return FVector::ZeroVector;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (auto TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}

	return FTaggedMontage();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return HealthComponent && HealthComponent->IsDeadOrDying();
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation() const
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

int32 AAuraCharacterBase::GetSummonCount_Implementation() const
{
	return SummonCount;
}

void AAuraCharacterBase::IncrementSummonCount_Implementation(int32 Amount)
{
	SummonCount += Amount;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void AAuraCharacterBase::SetBeingShocked_Implementation(bool bBeingShocked)
{
	bIsBeingShocked = bBeingShocked;
	UpdateMovementSpeedFromDebuffs();
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	UAuraAbilitySystemComponent* AuraASC =
		Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	if (!HasAuthority()) return;

	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance.Get()))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance.Get(), this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		StartDissolveTimeline(DynamicMatInst);
	}

	if (IsValid(WeaponDissolveMaterialInstance.Get()))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance.Get(), this);
		Weapon->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

float AAuraCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                     AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	// OnDamageDelegate는 ExecCalcDamage의 RadialDamage 캡처(GetOnDamageSignature().AddLambda)에서 사용 — 유지 필수.
	// HealthComponent::OnDamageTaken은 신규 BP 노출용.
	OnDamageDelegate.Broadcast(ActualDamage);
	HealthComponent->BroadcastDamageTaken(ActualDamage);
	return ActualDamage;
}
