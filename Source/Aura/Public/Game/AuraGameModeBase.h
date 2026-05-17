// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class USaveGame;
class UMVVMLoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
class ULoadScreenSaveGame;

/**
 *
 */
UCLASS

()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly, Category= "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category= "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	void SaveSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex);

	ULoadScreenSaveGame* GetLoadScreenSaveGame(const FString& SlotName, int32 SlotIndex) const;

	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

};
