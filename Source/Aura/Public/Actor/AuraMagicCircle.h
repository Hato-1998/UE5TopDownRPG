// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraMagicCircle.generated.h"

UCLASS()
class AURA_API AAuraMagicCircle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraMagicCircle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AuraMagicCircle")
	TObjectPtr<UDecalComponent> MagicCircleDecal;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
