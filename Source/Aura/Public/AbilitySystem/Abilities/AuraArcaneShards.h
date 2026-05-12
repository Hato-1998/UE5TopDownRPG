// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraArcaneShards.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArcaneShards")
	int32 MaxNumShards = 11;
};
