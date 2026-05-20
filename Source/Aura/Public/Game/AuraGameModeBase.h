// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UMVVMLoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
class ULoadScreenSaveGame;

/**
 *
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly, Category= "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category= "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoadScreenSaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	void SaveSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex);

	ULoadScreenSaveGame* GetLoadScreenSaveGame(const FString& SlotName, int32 SlotIndex) const;

	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	void TravelToMap(UMVVMLoadSlot* Slot);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	ULoadScreenSaveGame* RetrieveInGameSaveData();
	void SaveInGameSaveData(ULoadScreenSaveGame* SaveData);

	void SaveWorldState(UWorld* World) const;
	void LoadWorldState(UWorld* World) const;


protected:
	virtual void BeginPlay() override;

};
