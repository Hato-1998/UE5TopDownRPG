// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraCursorTargetingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraCursorTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void TraceCursor(bool bExcludePlayers);
	void ClearHighlightedActors();
	void OverrideMoveToLocationOnTarget(FVector& Destination) const;

	const FHitResult& GetCursorHit() const { return CursorHit; }
	AActor* GetTargetActor() const { return CurrentActor; }

private:
	void HighlightActor(AActor* InActor) const;
	void UnHighlightActor(AActor* InActor) const;

	UPROPERTY()
	TObjectPtr<AActor> LastActor;

	UPROPERTY()
	TObjectPtr<AActor> CurrentActor;

	FHitResult CursorHit;
};
