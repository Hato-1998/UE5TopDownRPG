// CopyrightHATO


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Launches a bolt of Fire Dealing: </><Damage>%d</>\n\n"
			"<Default> Fire Damage with a chance to burn</>"
		), Level, ManaCost, Cooldown, CurrentDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Launches %d bolts of Fire Dealing: </><Damage>%d</>\n\n"
			"<Default> Fire Damage with a chance to burn</>"
		), Level, static_cast<double>(ManaCost), static_cast<double>(Cooldown), FMath::Min(Level, NumProjectile), CurrentDamage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>FIRE BOLT</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Description
		"<Default>Launches %d bolts of Fire Dealing: </><Damage>%d</>\n\n"
		"<Default> Fire Damage with a chance to burn</>"
	), Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectile), CurrentDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float OverridePitch, AActor* HomingTarget)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Cannot spawn projectiles because AvatarActor is null."), __FUNCTION__);
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
		UE_LOG(LogAura, Warning, TEXT("%hs: Cannot spawn projectiles because World is null."), __FUNCTION__);
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
	FRotator Rotation = (TargetLocation - SpawnLocation).Rotation();

	if (bOverridePitch)	Rotation.Pitch = OverridePitch;

	const FVector Forward = Rotation.Vector();
	int32 EffectiveNumProjectile = FMath::Min(NumProjectile, GetAbilityLevel());
	const TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectile);

	for (const FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

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

		if (!ProjectileActor->ProjectileMovement)
		{
			UE_LOG(LogAura, Warning, TEXT("%hs: ProjectileMovement is null on spawned projectile %s."),
				__FUNCTION__, *GetNameSafe(ProjectileActor));
			ProjectileActor->Destroy();
			return;
		}

		ProjectileActor->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			ProjectileActor->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			USceneComponent* HomingScene = NewObject<USceneComponent>(ProjectileActor);
			HomingScene->SetWorldLocation(TargetLocation);
			HomingScene->RegisterComponent();
			ProjectileActor->HomingTargetSceneComponent = HomingScene;
			ProjectileActor->ProjectileMovement->HomingTargetComponent = HomingScene;
		}

		ProjectileActor->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(MinHomingAcceleration, MaxHomingAcceleration);
		ProjectileActor->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectile;

		ProjectileActor->FinishSpawning(SpawnTransform);
	}
}
