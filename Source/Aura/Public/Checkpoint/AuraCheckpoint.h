// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "AuraCheckpoint.generated.h"

class USphereComponent;
/**
 *
 */
UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveInterface
{
	GENERATED_BODY()

public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer);

	virtual bool ShouldLoadTransform_Implementation() override { return false;};
	virtual void LoadActor_Implementation() override;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

protected:
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* MaterialInstance);

	void HandleGlowEffects();

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
