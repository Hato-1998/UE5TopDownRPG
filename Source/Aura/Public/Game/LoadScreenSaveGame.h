// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	UPROPERTY()
	TArray<uint8> Byte;

};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "SavedAbility")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(BlueprintReadWrite, Category = "SavedAbility")
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, Category = "SavedAbility")
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, Category = "SavedAbility")
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, Category = "SavedAbility")
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, Category = "SavedAbility")
	int32 AbilityLevel = 1;
};

/** AbilityTag 기준 동치성.
 *  SaveAbilities는 SaveProgress 시 매번 Empty() 후 다시 채우므로
 *  AddUnique는 동일 어빌리티 중복 등록만 차단하는 용도다. */
inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

/**
 *
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString("Default Player Name");

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FString MapAssetName = FString("Default Asset Map Name");

	UPROPERTY()
	FName PlayerStartTag = FName("DefaultPlayerStart");

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus = Vacant;

	/** 이 슬롯에 대해 SaveProgress가 한 번도 호출되지 않은 초기 상태.
	 *  true  → 캐릭터 클래스 디폴트 어빌리티/속성 적용 (신규 게임 흐름).
	 *  false → SaveData에 저장된 어빌리티/속성을 복원. */
	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	/* player Data*/

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 PlayerXP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	float Strength = 0;

	UPROPERTY()
	float Intelligence = 0;

	UPROPERTY()
	float Resilience = 0;

	UPROPERTY()
	float Vigor = 0;

	UPROPERTY()
	TArray<FSavedAbility> SaveAbilities;

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName);

};
