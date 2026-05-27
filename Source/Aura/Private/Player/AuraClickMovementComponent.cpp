// CopyrightHATO


#include "Player/AuraClickMovementComponent.h"

#include "Components/SplineComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Player/AuraCursorTargetingComponent.h"

void UAuraClickMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Spline = NewObject<USplineComponent>(GetOwner(), TEXT("AutoRunSpline"));
	if (Spline)
	{
		Spline->RegisterComponent();
	}
}

void UAuraClickMovementComponent::StopAutoRun()
{
	bAutoRunning = false;
}

void UAuraClickMovementComponent::AddFollowTime(float DeltaSeconds)
{
	FollowTime += DeltaSeconds;
}

void UAuraClickMovementComponent::ResetFollowTime()
{
	FollowTime = 0.f;
}

void UAuraClickMovementComponent::UpdateCachedDestination(
	const FHitResult& CursorHit,
	const UAuraCursorTargetingComponent* CursorTargetingComponent)
{
	if (!CursorHit.bBlockingHit) return;

	FVector Destination = CursorHit.ImpactPoint;
	if (CursorTargetingComponent)
	{
		CursorTargetingComponent->OverrideMoveToLocationOnTarget(Destination);
	}

	CachedDestination = Destination;
	bHasCachedDestination = true;
}

void UAuraClickMovementComponent::MoveTowardCachedDestination(APawn* ControlledPawn) const
{
	if (!ControlledPawn || !bHasCachedDestination) return;

	const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
	ControlledPawn->AddMovementInput(WorldDirection);
}

void UAuraClickMovementComponent::TryStartAutoRun(
	UObject* WorldContextObject,
	APawn* ControlledPawn,
	bool bSpawnClickEffect,
	UNiagaraSystem* ClickNiagaraSystem)
{
	if (!ControlledPawn || !bHasCachedDestination || !IsShortPress()) return;

	if (Spline)
	{
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			WorldContextObject, ControlledPawn->GetActorLocation(), CachedDestination))
		{
			Spline->ClearSplinePoints();
			const TArray<FVector>& PathPoints = NavPath->PathPoints;
			for (const FVector& PointLoc : PathPoints)
			{
				Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
			}
			if (PathPoints.Num() > 0)
			{
				CachedDestination = PathPoints.Last();
				bAutoRunning = true;
			}
		}
	}

	if (bSpawnClickEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, ClickNiagaraSystem, CachedDestination);
	}
}

void UAuraClickMovementComponent::TickAutoRun(APawn* ControlledPawn)
{
	if (!bAutoRunning || !ControlledPawn || !Spline) return;

	const FVector LocationOnSpline =
		Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);

	const FVector Direction =
		Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline, ESplineCoordinateSpace::World);

	ControlledPawn->AddMovementInput(Direction);

	const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
	if (DistanceToDestination <= AutoRunAcceptanceRadius)
	{
		bAutoRunning = false;
	}
}
