// CopyrightHATO


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SpawnLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AAuraProjectile* ProjectileActor = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	const FGameplayEffectSpecHandle DamageEffectSpec = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());;

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec, GameplayTags.Damage, ScaledDamage);
	ProjectileActor->DamageEffectSpecHandle = DamageEffectSpec;

	ProjectileActor->FinishSpawning(SpawnTransform);
}
