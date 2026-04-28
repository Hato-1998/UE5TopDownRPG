// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton Containing Native Gameplay Tags
 */

struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	FGameplayTag Attribute_Primary_Strength;
	FGameplayTag Attribute_Primary_Intelligence;
	FGameplayTag Attribute_Primary_Resilience;
	FGameplayTag Attribute_Primary_Vigor;

	FGameplayTag Attribute_Secondary_Armor;
	FGameplayTag Attribute_Secondary_ArmorPenetration;
	FGameplayTag Attribute_Secondary_BlockChance;
	FGameplayTag Attribute_Secondary_CriticalHitChance;
	FGameplayTag Attribute_Secondary_CriticalHitDamage;
	FGameplayTag Attribute_Secondary_CriticalHitReduction;
	FGameplayTag Attribute_Secondary_HealthRegeneration;
	FGameplayTag Attribute_Secondary_ManaRegeneration;
	FGameplayTag Attribute_Secondary_MaxHealth;
	FGameplayTag Attribute_Secondary_MaxMana;

	FGameplayTag Attribute_Secondary_ResFire;
	FGameplayTag Attribute_Secondary_ResLightning;
	FGameplayTag Attribute_Secondary_ResArcane;
	FGameplayTag Attribute_Secondary_ResPhysical;

	FGameplayTag Attribute_Vital_Health;
	FGameplayTag Attribute_Vital_Mana;

	FGameplayTag Attribute_Meta_IncomingXP;

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_Action1;
	FGameplayTag InputTag_Action2;
	FGameplayTag InputTag_Action3;
	FGameplayTag InputTag_Action4;
	FGameplayTag InputTag_Shift;
	FGameplayTag InputTag_Passive1;
	FGameplayTag InputTag_Passive2;

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;
	FGameplayTag Abilities_HitReact;

	FGameplayTag Effects_HitReact;

	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;

	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Cooldown_Fire_FireBolt;

	/*
	 * CombatSocket
	 */
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	/*
	 * Montage Tags
	 */
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	/*
	 * Root Category Tags (for MatchesTag comparisons)
	 */
	FGameplayTag Abilities;
	FGameplayTag Input;
	FGameplayTag Message;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

protected:

private:
	static FAuraGameplayTags GameplayTags;
};
