// CopyrightHATO


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BLAST</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			"<Small>Launches: %d\n\n"

			// Description
			"<Default> Fire Dealing: </><Damage>%d</>\n\n"
			"<Default> Fire Radial Damage with a chance to burn</>"
		), Level, ManaCost, Cooldown, NumFireBalls, CurrentDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BLAST</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			"<Small>Launches: %d\n\n"

			// Description
			"<Default> Fire Dealing: </><Damage>%d</>\n\n"
			"<Default> Fire Radial Damage with a chance to burn</>"
		), Level, static_cast<double>(ManaCost), static_cast<double>(Cooldown), NumFireBalls, CurrentDamage);
	}
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>Next Level</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Small>Launches: %d\n\n"

		// Description
		"<Default> Fire Dealing: </><Damage>%d</>\n\n"
		"<Default> Fire Radial Damage with a chance to burn</>"
	), Level, ManaCost, Cooldown, NumFireBalls, CurrentDamage);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBall()
{
	TArray<AAuraFireBall*> FireBalls;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);

	for (const FRotator& Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBlastClass,
			SpawnTransform,
			GetAvatarActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		FireBalls.Add(FireBall);

		FireBall->FinishSpawning(SpawnTransform);
	}

	return FireBalls;
}
