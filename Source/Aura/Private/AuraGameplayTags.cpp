// CopyrightHATO


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Primary Attributes
	 */
	GameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Strength"), FString("Increases Physical Damage"));

	GameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Intelligence"), FString("Increases Magical Damage and Max Mana"));

	GameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Resilience"), FString("Increases Armor and Armor Penetration"));

	GameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Vigor"), FString("Increases Max Health and Health Regeneration"));

	/*
	 * Secondary Attributes
	 */
	GameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.Armor"), FString("Reduce Damage Taken, Improves Block Chance"));

	GameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.ArmorPenetration"),
		FString("Ignores Percentage of Enemy Armor, Increases Critical Hit Chance"));

	GameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.BlockChance"), FString("Chance to Cut Incoming Damage in Half"));

	GameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitChance"), FString("Chance to Double Damage Plus Critical Hit Bonus"));

	GameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitDamage"), FString("Bonus Damage Added When a Critical Hit is Scored"));

	GameplayTags.Attribute_Secondary_CriticalHitReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitReduction"), FString("Reduces Critical Hit Chance of Attacking Enemies"));

	GameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.HealthRegeneration"), FString("Amount of Health Regenerated Every Second"));

	GameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.ManaRegeneration"), FString("Amount of Mana Regenerated Every Second"));

	GameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.MaxHealth"), FString("Maximum Amount of Health Obtainable"));

	GameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.MaxMana"), FString("Maximum Amount of Mana Obtainable"));

	/*
	 * Vital Attributes
	 */
	GameplayTags.Attribute_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.Health"), FString("Current Health of the Character"));

	GameplayTags.Attribute_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.Mana"), FString("Current Mana of the Character"));

	/*
	* Input Tag
	*/
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Mouse.LMB"), FString("Input Tag for Left Mouse Button"));

	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Mouse.RMB"), FString("Input Tag for Right Mouse Button"));

	GameplayTags.InputTag_Action1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Button.Action1"), FString("Input Tag for Action 1"));

	GameplayTags.InputTag_Action2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Button.Action2"), FString("Input Tag for Action 2"));

	GameplayTags.InputTag_Action3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Button.Action3"), FString("Input Tag for Action 3"));

	GameplayTags.InputTag_Action4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Button.Action4"), FString("Input Tag for Action 4"));

	GameplayTags.InputTag_Shift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Button.Shift"), FString("Input Tag for Shift"));

	/*
	 * Damage Types
	 */

	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"), FString("Damage"));

	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Fire"), FString("Fire Damage Type"));

	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Lightning"), FString("Lightning Damage Type"));

	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Arcane"), FString("Arcane Damage Type"));

	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Physical"), FString("Physical Damage Type"));

	/*
	 * Resistance Types
	 */

	GameplayTags.Attribute_Secondary_ResFire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attribute.Secondary.Resistance.Fire"), FString("Fire Resistance Type"));

	GameplayTags.Attribute_Secondary_ResLightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attribute.Secondary.Resistance.Lightning"), FString("Lightning Resistance Type"));

	GameplayTags.Attribute_Secondary_ResArcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attribute.Secondary.Resistance.Arcane"), FString("Arcane Resistance Type"));

	GameplayTags.Attribute_Secondary_ResPhysical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attribute.Secondary.Resistance.Physical"), FString("Physical Resistance Type"));

	/*
	 * Map of Damage Types to Resistance Types
	 */

	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attribute_Secondary_ResFire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attribute_Secondary_ResLightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attribute_Secondary_ResArcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attribute_Secondary_ResPhysical);

	/*
	 * Global Tag
	 */

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effects.HitReact"), FString("Effects tag for HitReact"));

	/*
	 * Abilities
	 */

	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Attack"), FString("Attack Abilities Tag"));

	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Summon"), FString("Summon Abilities Tag"));

	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Fire.FireBolt"), FString("Fire Bolt Abilities Tag"));

	/*
	 * Cooldown
	 */

	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Fire.FireBolt"), FString("Fire Bolt Cooldown Tag"));


	/*
	* CombatSocket
	*/

	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Weapon"), FString("CombatSocket Tag"));

	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.RightHand"), FString("CombatSocket Tag"));

	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.LeftHand"), FString("CombatSocket Tag"));

	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Tail"), FString("CombatSocket Tag"));

	/*
	 * Montage Tags
	 */

	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack_1"), FString("Montage Attack Tag"));
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack_2"), FString("Montage Attack Tag"));
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack_3"), FString("Montage Attack Tag"));
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack_4"), FString("Montage Attack Tag"));

}
