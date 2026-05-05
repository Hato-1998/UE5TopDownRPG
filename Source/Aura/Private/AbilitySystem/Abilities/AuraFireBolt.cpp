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

	const bool bIsServer = AvatarActor->HasAuthority();
	if (!bIsServer) return;

	UWorld* World = GetWorld();

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

		ProjectileActor->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			ProjectileActor->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			ProjectileActor->HomingTargetSceneComponent =NewObject<USceneComponent>(USceneComponent::StaticClass());
			ProjectileActor->HomingTargetSceneComponent->SetWorldLocation(TargetLocation);
			ProjectileActor->ProjectileMovement->HomingTargetComponent = ProjectileActor->HomingTargetSceneComponent;
		}

		ProjectileActor->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(MinHomingAcceleration, MaxHomingAcceleration);
		ProjectileActor->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectile;

		ProjectileActor->FinishSpawning(SpawnTransform);
	}
}
