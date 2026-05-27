// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AuraGameDataSubsystem.generated.h"

class UAbilityInfo;
class UCharacterClassInfo;
class ULootTiers;

/**
 * Central access point for project-wide RPG data assets.
 *
 * GameMode is server-only, so shared data that UI, GAS calculations, and tests may need
 * should be reachable from the GameInstance lifetime instead.
 */
UCLASS(Config=Game)
class AURA_API UAuraGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Aura|Game Data")
	UCharacterClassInfo* GetCharacterClassInfo();

	UFUNCTION(BlueprintPure, Category="Aura|Game Data")
	UAbilityInfo* GetAbilityInfo();

	UFUNCTION(BlueprintPure, Category="Aura|Game Data")
	ULootTiers* GetLootTiers();

private:
	template <typename AssetType>
	AssetType* LoadConfiguredAsset(const FSoftObjectPath& AssetPath, TObjectPtr<AssetType>& CachedAsset, const TCHAR* AssetLabel);

	UPROPERTY(Config, EditDefaultsOnly, Category="Aura|Game Data")
	FSoftObjectPath CharacterClassInfoPath;

	UPROPERTY(Config, EditDefaultsOnly, Category="Aura|Game Data")
	FSoftObjectPath AbilityInfoPath;

	UPROPERTY(Config, EditDefaultsOnly, Category="Aura|Game Data")
	FSoftObjectPath LootTiersPath;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterClassInfo> CachedCharacterClassInfo;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityInfo> CachedAbilityInfo;

	UPROPERTY(Transient)
	TObjectPtr<ULootTiers> CachedLootTiers;
};
