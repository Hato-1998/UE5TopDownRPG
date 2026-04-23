// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AuraLevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttributePointReward = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SpellPointReward = 1;
};

/**
 *
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;

	/** 주어진 경험치가 도달하는 레벨을 반환. LevelUpInformation[0]은 dummy, 1레벨부터 유효. */
	int32 FindLevelForXP(int32 XP) const;

	/** 현재 Level과 XP를 받아 레벨 바 진행도(0.0~1.0) 반환. */
	float GetXPBarPercent(int32 Level, int32 XP) const;
};
