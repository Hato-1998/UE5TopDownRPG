// CopyrightHATO


#include "AbilitySystem/Data/AttributeInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeEntries)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error,
			TEXT("AttributeInfo: Could not find metadata for tag %s in %s. Entries=%d. Check DA_AttributeInfo coverage."),
			*AttributeTag.ToString(),
			*GetNameSafe(this),
			AttributeEntries.Num())
	}

	return FAuraAttributeInfo();
}
