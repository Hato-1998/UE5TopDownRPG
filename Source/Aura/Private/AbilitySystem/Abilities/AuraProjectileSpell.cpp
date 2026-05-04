// CopyrightHATO


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Aura/AuraLogChannels.h"
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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float OverridePitch)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Cannot spawn projectile because AvatarActor is null."), __FUNCTION__);
		return;
	}

	const bool bIsServer = AvatarActor->HasAuthority();
	if (!bIsServer) return;

	if (!ProjectileClass)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: ProjectileClass is not set on %s."), __FUNCTION__, *GetNameSafe(this));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Cannot spawn projectile because World is null."), __FUNCTION__);
		return;
	}

	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);
	if (!CombatInterface)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: AvatarActor %s does not implement CombatInterface."),
			__FUNCTION__, *GetNameSafe(AvatarActor));
		return;
	}

	const FVector SpawnLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		AvatarActor,
		SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();

	if (bOverridePitch)
	{
		Rotation.Pitch = OverridePitch;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AAuraProjectile* ProjectileActor = World->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!ProjectileActor)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: SpawnActorDeferred failed for ProjectileClass %s."),
			__FUNCTION__, *GetNameSafe(ProjectileClass));
		return;
	}

	ProjectileActor->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

	ProjectileActor->FinishSpawning(SpawnTransform);
}
