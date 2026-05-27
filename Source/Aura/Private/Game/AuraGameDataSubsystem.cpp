// CopyrightHATO


#include "Game/AuraGameDataSubsystem.h"

#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Data/LootTiers.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"

template <typename AssetType>
AssetType* UAuraGameDataSubsystem::LoadConfiguredAsset(const FSoftObjectPath& AssetPath, TObjectPtr<AssetType>& CachedAsset, const TCHAR* AssetLabel)
{
	if (CachedAsset)
	{
		return CachedAsset;
	}

	if (AssetPath.IsNull())
	{
		return nullptr;
	}

	UObject* LoadedObject = AssetPath.TryLoad();
	CachedAsset = Cast<AssetType>(LoadedObject);
	if (!CachedAsset)
	{
		UE_LOG(LogAura, Error, TEXT("AuraGameDataSubsystem: Failed to load %s from '%s'"), AssetLabel, *AssetPath.ToString());
	}

	return CachedAsset;
}

UCharacterClassInfo* UAuraGameDataSubsystem::GetCharacterClassInfo()
{
	if (UCharacterClassInfo* ConfiguredAsset = LoadConfiguredAsset(CharacterClassInfoPath, CachedCharacterClassInfo, TEXT("CharacterClassInfo")))
	{
		return ConfiguredAsset;
	}

	if (UWorld* World = GetWorld())
	{
		if (const AAuraGameModeBase* AuraGameMode = World->GetAuthGameMode<AAuraGameModeBase>())
		{
			return AuraGameMode->CharacterClassInfo;
		}
	}

	return nullptr;
}

UAbilityInfo* UAuraGameDataSubsystem::GetAbilityInfo()
{
	if (UAbilityInfo* ConfiguredAsset = LoadConfiguredAsset(AbilityInfoPath, CachedAbilityInfo, TEXT("AbilityInfo")))
	{
		return ConfiguredAsset;
	}

	if (UWorld* World = GetWorld())
	{
		if (const AAuraGameModeBase* AuraGameMode = World->GetAuthGameMode<AAuraGameModeBase>())
		{
			return AuraGameMode->AbilityInfo;
		}
	}

	return nullptr;
}

ULootTiers* UAuraGameDataSubsystem::GetLootTiers()
{
	if (ULootTiers* ConfiguredAsset = LoadConfiguredAsset(LootTiersPath, CachedLootTiers, TEXT("LootTiers")))
	{
		return ConfiguredAsset;
	}

	if (UWorld* World = GetWorld())
	{
		if (const AAuraGameModeBase* AuraGameMode = World->GetAuthGameMode<AAuraGameModeBase>())
		{
			return AuraGameMode->LootTiers;
		}
	}

	return nullptr;
}
