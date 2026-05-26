// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/AuraCheckpoint.h"
#include "AuraMapEntrance.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraMapEntrance : public AAuraCheckpoint
{
	GENERATED_BODY()

public:
	AAuraMapEntrance(const FObjectInitializer& ObjectInitializer);

	virtual void HighLightActor_Implementation() override;

	virtual void LoadActor_Implementation() override;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
