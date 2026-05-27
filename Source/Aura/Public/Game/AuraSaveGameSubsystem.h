// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AuraSaveGameSubsystem.generated.h"

class ULoadScreenSaveGame;
class UMVVMLoadSlot;
class USaveGame;
class AAuraGameModeBase;
class AActor;

/**
 * Owns save slot and world-state persistence for the Aura runtime.
 *
 * GameMode remains the authority-facing facade, but persistence policy belongs here so
 * menu, travel, and gameplay code can eventually call the same service directly.
 */
UCLASS()
class AURA_API UAuraSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool CreateNewLoadSlot(UMVVMLoadSlot* LoadSlot, int32 SlotIndex, const FString& PlayerName) const;
	bool LoadSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex) const;
	void DeleteLoadSlot(UMVVMLoadSlot* LoadSlot) const;
	bool TravelToLoadSlot(UMVVMLoadSlot* LoadSlot) const;

	ULoadScreenSaveGame* RetrieveInGameSaveData() const;
	void SaveInGameSaveData(ULoadScreenSaveGame* SaveData) const;
	void SavePlayerProgress(AActor* PlayerActor, FName CheckpointTag) const;
	void HandlePlayerDeath(AActor* DeadActor) const;

	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString()) const;
	void LoadWorldState(UWorld* World) const;

private:
	AAuraGameModeBase* GetAuraGameMode() const;
	void SaveSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex, TSubclassOf<ULoadScreenSaveGame> SaveGameClass,
		const FString& DefaultMapName, const TSoftObjectPtr<UWorld>& DefaultMap, FName DefaultPlayerStartTag) const;
	ULoadScreenSaveGame* GetLoadScreenSaveGame(const FString& SlotName, int32 SlotIndex,
		TSubclassOf<ULoadScreenSaveGame> SaveGameClass) const;
	ULoadScreenSaveGame* RetrieveInGameSaveData(TSubclassOf<ULoadScreenSaveGame> SaveGameClass) const;
	void SaveWorldState(UWorld* World, TSubclassOf<ULoadScreenSaveGame> SaveGameClass,
		const TMap<FString, TSoftObjectPtr<UWorld>>& Maps, const FString& DestinationMapAssetName = FString()) const;
	void LoadWorldState(UWorld* World, TSubclassOf<ULoadScreenSaveGame> SaveGameClass) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	static bool SaveToSlot(USaveGame* SaveGame, const FString& SlotName, int32 SlotIndex, const TCHAR* Context);
	static bool FindMapNameFromMapAssetName(const TMap<FString, TSoftObjectPtr<UWorld>>& Maps, const FString& MapAssetName, FString& OutMapName);
	bool SetCurrentLoadSlot(const UMVVMLoadSlot* LoadSlot) const;
	bool GetCurrentSlot(FString& OutSlotName, int32& OutSlotIndex) const;
};
