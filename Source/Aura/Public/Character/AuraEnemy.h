// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/HighLightInterface.h"
#include "AuraEnemy.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighLightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;

	virtual int32 GetPlayerLevel() const override;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaluts")
	int32 Level = 1;
};
