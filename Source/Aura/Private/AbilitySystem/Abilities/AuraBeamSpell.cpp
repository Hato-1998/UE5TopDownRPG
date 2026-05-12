// CopyrightHATO


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();

		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitActor;
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPC = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* WeaponMesh = ICombatInterface::Execute_GetWeaponMesh(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;

			const FVector SocketLocation = WeaponMesh->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.f,
				ETraceTypeQuery::TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult, true);

			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}

	if (MouseHitActor && !UAuraAbilitySystemLibrary::IsNotFriend(OwnerCharacter, MouseHitActor))
	{
		MouseHitActor = nullptr;
		MouseHitLocation = BeamTargetLocation;
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	UAuraAbilitySystemLibrary::GetLivePlayerWithInRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation(),
		GetAvatarActorFromActorInfo());

	int NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);

	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}

}

void UAuraBeamSpell::UnbindDeathDelegates(AActor* PrimaryTarget, const TArray<AActor*>& AdditionalTargets)
{
	if (IsValid(PrimaryTarget))
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(PrimaryTarget))
		{
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}

	for (AActor* Target : AdditionalTargets)
	{
		if (!IsValid(Target)) continue;
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
		}
	}
}


FString UAuraBeamSpell::GetDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>ELECTROCUTE</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Emits a beam of lightning, connecting with the target, dealing: </><Damage>%d</>\n\n"
			"<Default> Lightning Damage with a chance to Stun</>"
		), Level, ManaCost, Cooldown, CurrentDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>ELECTROCUTE</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Emits a beam of lightning, also chaining to </><Damage>%d</><Default> additional nearby targets, dealing: </><Damage>%d</>\n\n"
			"<Default> Lightning Damage each with a chance to Stun</>"
		), Level, ManaCost, Cooldown, FMath::Min(Level - 1, MaxNumShockTargets), CurrentDamage);
	}
}

FString UAuraBeamSpell::GetNextLevelDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>ELECTROCUTE</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Description
		"<Default>Emits a beam of lightning, also chaining to </><Damage>%d</><Default> additional nearby targets, dealing: </><Damage>%d</>\n\n"
		"<Default> Lightning Damage each with a chance to Stun</>"
	), Level, ManaCost, Cooldown, FMath::Min(Level - 1, MaxNumShockTargets), CurrentDamage);
}
