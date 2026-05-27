// CopyrightHATO

#pragma once

#include "CoreMinimal.h"

namespace AuraSaveDefaults
{
	inline constexpr const TCHAR* DefaultPlayerName = TEXT("Default Player Name");
	inline constexpr const TCHAR* DefaultMapName = TEXT("Default Map Name");
	inline constexpr const TCHAR* DefaultMapAssetName = TEXT("Default Asset Map Name");
	inline constexpr const TCHAR* DefaultPlayerStartTag = TEXT("DefaultPlayerStart");
	inline constexpr const TCHAR* FallbackSlotName = TEXT("LoadSlot_0");
	inline constexpr int32 MaxLoadSlots = 3;

	inline FString GetSlotNameForIndex(int32 SlotIndex)
	{
		return FString::Printf(TEXT("LoadSlot_%d"), SlotIndex);
	}
}
