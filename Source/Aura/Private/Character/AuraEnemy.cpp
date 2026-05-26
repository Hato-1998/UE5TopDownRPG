// CopyrightHATO


#include "Character/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraHealthComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AI/AuraAIController.h"
#include "AI/AuraAIBlackboardKeys.h"
#include "Aura/Aura.h"
#include "Aura/AuraLogChannels.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/Widget/EnemyHealthBarWidgetController.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	BaseWalkSpeed = 250.f;

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	AuraAIController = Cast<AAuraAIController>(NewController);
	if (!AuraAIController)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: NewController on %s is not an AAuraAIController."),
			__FUNCTION__, *GetNameSafe(this));
		return;
	}

	if (!BehaviorTree)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: BehaviorTree is not set on %s."),
			__FUNCTION__, *GetNameSafe(this));
		return;
	}

	if (!BehaviorTree->BlackboardAsset)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: BehaviorTree %s has no BlackboardAsset."),
			__FUNCTION__, *GetNameSafe(BehaviorTree));
		return;
	}

	UBlackboardComponent* BlackboardComp = AuraAIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: BlackboardComponent is null on %s."),
			__FUNCTION__, *GetNameSafe(AuraAIController));
		return;
	}

	BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	BlackboardComp->SetValueAsBool(AuraBBKeys::HitReacting, false);
	BlackboardComp->SetValueAsBool(AuraBBKeys::RangedAttacker, CharacterClass != ECharacterClass::Warrior);
}

void AAuraEnemy::HighLightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighLightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);

	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel_Implementation() const
{
	return Level;
}

int32 AAuraEnemy::GetXPReward_Implementation() const
{
	return XPReward.GetValueAtLevel(Level);
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	HealthBar->SetVisibility(false);
	SetLifeSpan(LifeSpan);

	if (AuraAIController) AuraAIController->GetBlackboardComponent()->SetValueAsBool(AuraBBKeys::Dead, true);

	if (HasAuthority())
	{
		SpawnLoot();
	}

	Super::Die(DeathImpulse);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	TArray<UPassiveNiagaraComponent*> PassiveComps;
	GetComponents<UPassiveNiagaraComponent>(PassiveComps);
	for (UPassiveNiagaraComponent* Comp : PassiveComps)
	{
		Comp->BindToASC(AbilitySystemComponent);
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(
	FAuraGameplayTags::Get().Debuff_Stun,
	EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::StunTagChanged);

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (HealthComponent)
	{
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
	}

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		if (EnemyHealthBarWidgetControllerClass)
		{
			UEnemyHealthBarWidgetController* WC = NewObject<UEnemyHealthBarWidgetController>(this, EnemyHealthBarWidgetControllerClass);
			WC->SetEnemyHealthComponent(HealthComponent);
			WC->BindCallbacksToDependencies();
			AuraUserWidget->SetWidgetController(WC);
			WC->BroadcastInitialValues();
		}
		else
		{
			// fallback: 기존처럼 self를 controller로 (BP가 아직 마이그레이션 안 된 경우)
			AuraUserWidget->SetWidgetController(this);
		}
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &AAuraEnemy::HitReactTagChanged);

	OnAscRegistered.Broadcast(AbilitySystemComponent);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;

	UpdateMovementSpeedFromDebuffs();

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(AuraBBKeys::HitReacting, bHitReacting);
	}
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	Super::StunTagChanged(CallBackTag, NewCount);

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(AuraBBKeys::Stunned, bIsStunned);
	}
}

void AAuraEnemy::SetBeingShocked_Implementation(bool bBeingShocked)
{
	Super::SetBeingShocked_Implementation(bBeingShocked);

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(AuraBBKeys::BeingShocked, bBeingShocked);
	}
}

void AAuraEnemy::UpdateMovementSpeedFromDebuffs()
{
	const bool bMovementLocked = bIsStunned || bIsBeingShocked || bHitReacting;
	GetCharacterMovement()->MaxWalkSpeed = bMovementLocked ? 0.f : BaseWalkSpeed;
}
