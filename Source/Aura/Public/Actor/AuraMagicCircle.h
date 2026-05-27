// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraMagicCircle.generated.h"

class UDecalComponent;
class USceneComponent;

UCLASS()
class AURA_API AAuraMagicCircle : public AActor
{
	GENERATED_BODY()

public:
	AAuraMagicCircle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AuraMagicCircle")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AuraMagicCircle")
	TObjectPtr<UDecalComponent> MagicCircleDecal;
};
