// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
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

	UPROPERTY(EditDefaultsOnly, Category= "Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

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

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	virtual void BeginPlay() override;

};
