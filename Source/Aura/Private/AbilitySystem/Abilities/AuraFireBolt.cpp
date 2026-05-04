// CopyrightHATO


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Launches a bolt of Fire Dealing: </><Damage>%d</>\n\n"
			"<Default> Fire Damage with a chance to burn</>"
		), Level, ManaCost, Cooldown, CurrentDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Launches %d bolts of Fire Dealing: </><Damage>%d</>\n\n"
			"<Default> Fire Damage with a chance to burn</>"
		), Level, static_cast<double>(ManaCost), static_cast<double>(Cooldown), FMath::Min(Level, NumProjectile), CurrentDamage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>FIRE BOLT</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Description
		"<Default>Launches %d bolts of Fire Dealing: </><Damage>%d</>\n\n"
		"<Default> Fire Damage with a chance to burn</>"
	), Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectile), CurrentDamage);
}
