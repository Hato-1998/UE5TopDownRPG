// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning",
		meta=(ClampMin="0.0", ClampMax="100.0", UIMin="0.0", UIMax="100.0"))
	float ChanceToSpawn = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning",
		meta=(ClampMin="0"))
	int32 MaxNumberToSpawn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/**
 *
 */
UCLASS()
class AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "LootTiers|Spawning")
	TArray<FLootItem> GetLootItems() const;

	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	TArray<FLootItem> LootItems;


};
