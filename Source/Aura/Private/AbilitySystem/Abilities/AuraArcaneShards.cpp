// CopyrightHATO


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>ARCANE SHARDS</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Summon a shard of arcane energy at the target location, dealing: </><Damage>%d</>\n\n"
			"<Default> radial Arcane Damage at the impact point</>"
		), Level, ManaCost, Cooldown, CurrentDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>ARCANE SHARDS</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Summon </><Damage>%d</><Default> shards of arcane energy at random locations inside the target area, each dealing: </><Damage>%d</>\n\n"
			"<Default> radial Arcane Damage at the impact point</>"
		), Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), CurrentDamage);
	}
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 CurrentDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>ARCANE SHARDS</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Description
		"<Default>Summon </><Damage>%d</><Default> shards of arcane energy at random locations inside the target area, each dealing: </><Damage>%d</>\n\n"
		"<Default> radial Arcane Damage at the impact point</>"
	), Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), CurrentDamage);
}
