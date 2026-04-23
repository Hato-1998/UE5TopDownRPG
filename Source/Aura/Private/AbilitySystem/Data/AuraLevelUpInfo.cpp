// CopyrightHATO


#include "AbilitySystem/Data/AuraLevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		if (LevelUpInformation.Num() - 1 <= Level) return Level;
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}

float ULevelUpInfo::GetXPBarPercent(int32 Level, int32 XP) const
{
	// 유효성 검사: Level < 1 또는 배열 크기 부족 시 0 반환
	if (Level < 1 || Level >= LevelUpInformation.Num()) return 0.f;

	const int32 PrevLevelRequirement = LevelUpInformation[Level - 1].LevelUpRequirement;
	const int32 NextLevelRequirement = LevelUpInformation[Level].LevelUpRequirement;
	const int32 DeltaLevelRequirement = NextLevelRequirement - PrevLevelRequirement;

	// 분모 0 방지 (잘못 구성된 DataAsset)
	if (DeltaLevelRequirement <= 0) return 0.f;

	const int32 XPInCurrentLevel = XP - PrevLevelRequirement;
	return static_cast<float>(XPInCurrentLevel) / static_cast<float>(DeltaLevelRequirement);
}
