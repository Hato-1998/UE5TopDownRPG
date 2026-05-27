// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraClickMovementComponent.generated.h"

class UAuraCursorTargetingComponent;
class UNiagaraSystem;
class USplineComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraClickMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void StopAutoRun();
	void AddFollowTime(float DeltaSeconds);
	void ResetFollowTime();
	void UpdateCachedDestination(const FHitResult& CursorHit, const UAuraCursorTargetingComponent* CursorTargetingComponent);
	void MoveTowardCachedDestination(APawn* ControlledPawn) const;
	void TryStartAutoRun(UObject* WorldContextObject, APawn* ControlledPawn, bool bSpawnClickEffect, UNiagaraSystem* ClickNiagaraSystem);
	void TickAutoRun(APawn* ControlledPawn);

private:
	bool IsShortPress() const { return FollowTime <= ShortPressThreshold; }

	UPROPERTY(EditDefaultsOnly, Category = "Click Movement")
	float ShortPressThreshold = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Click Movement")
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY()
	TObjectPtr<USplineComponent> Spline;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	bool bHasCachedDestination = false;
};
