// CopyrightHATO


#include "AbilitySystem/Data/CharacterClassInfo.h"

#include "Aura/AuraLogChannels.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass) const
{
	if (const FCharacterClassDefaultInfo* ClassDefaultInfo = CharacterClassInformation.Find(CharacterClass))
	{
		return *ClassDefaultInfo;
	}

	UE_LOG(LogAura, Error, TEXT("CharacterClassInfo: Missing class default info for %s in %s"),
		*UEnum::GetValueAsString(CharacterClass), *GetNameSafe(this));
	return FCharacterClassDefaultInfo();
}
