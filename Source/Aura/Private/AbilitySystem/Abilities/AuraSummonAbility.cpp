// CopyrightHATO


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSummonLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumSummons;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SummonLocations;
	for (int32 i = 0; i < NumSummons; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult OutHitResult;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			ChosenSpawnLocation + FVector(0.f, 0.f, 400.f),
			ChosenSpawnLocation - FVector(0.f,0.f, 400.f),
			ECC_Visibility
		);

		if (OutHitResult.bBlockingHit)
		{
			ChosenSpawnLocation = OutHitResult.ImpactPoint;
		}

		SummonLocations.Add(ChosenSpawnLocation);
	}

	return SummonLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomSummonClass()
{
	int32 Selection = FMath::RandRange(0, SummonedClasses.Num() - 1);
	return SummonedClasses[Selection];
}
