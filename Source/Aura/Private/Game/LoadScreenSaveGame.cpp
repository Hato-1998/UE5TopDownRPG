// CopyrightHATO


#include "Game/LoadScreenSaveGame.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	if (const FSavedMap* SavedMap = FindSavedMapWithMapName(InMapName))
	{
		return *SavedMap;
	}

	return FSavedMap();
}

const FSavedMap* ULoadScreenSaveGame::FindSavedMapWithMapName(const FString& InMapName) const
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapAssetName == InMapName)
		{
			return &SavedMap;
		}
	}

	return nullptr;
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	return FindSavedMapWithMapName(InMapName) != nullptr;
}
