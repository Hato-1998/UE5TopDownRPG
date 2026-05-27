// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraMagicCircleComponent.generated.h"

class AAuraMagicCircle;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraMagicCircleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void ShowMagicCircle(TSubclassOf<AAuraMagicCircle> MagicCircleClass, UMaterialInterface* DecalMaterial = nullptr);
	void HideMagicCircle();
	void UpdateMagicCircleLocation(const FHitResult& CursorHit) const;

	bool IsShowingMagicCircle() const;

private:
	UPROPERTY()
	TObjectPtr<AAuraMagicCircle> MagicCircle;
};
