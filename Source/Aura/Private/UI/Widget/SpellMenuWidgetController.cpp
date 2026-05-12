// CopyrightHATO


#include "UI/Widget/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	OnSpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
	CurrentSpellPoints = GetAuraPS()->GetSpellPoints();
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;

			BroadcastSpellGlobeSelected();
		}

		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 NewSpellPoints)
		{
			OnSpellPointsChangedDelegate.Broadcast(NewSpellPoints);
			CurrentSpellPoints = GetAuraPS()->GetSpellPoints();

			BroadcastSpellGlobeSelected();
		});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipSelection)
	{
		if (AbilityInfo)
		{
			const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
			StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		}
		bWaitForEquipSelection = false;
	}

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;

	if (!bSpecValid || !bTagValid || bTagNone)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;

	BroadcastSpellGlobeSelected();
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
}

void USpellMenuWidgetController::GlobeDeselected()
{
	if (bWaitForEquipSelection)
	{
		if (AbilityInfo)
		{
			const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
			StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		}
		bWaitForEquipSelection = false;
	}

	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	if (!AbilityInfo) return;

	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipSelectionDelegate.Broadcast(AbilityType);

	bWaitForEquipSelection = true;

	const FGameplayTag SelectedAbilityStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedAbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetSlotFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitForEquipSelection) return;
	if (!AbilityInfo) return;

	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitForEquipSelection = false;

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (!AbilityInfo) return;

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	LastSlotInfo.InputTag = PreviousSlot;

	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo NewSlotInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	NewSlotInfo.StatusTag = StatusTag;
	NewSlotInfo.InputTag = Slot;

	AbilityInfoDelegate.Broadcast(NewSlotInfo);

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignDelegate.Broadcast(AbilityTag);
	GlobeDeselected();
}

void USpellMenuWidgetController::ShouldEnableButton(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                    bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	bShouldEnableSpellPointsButton = false;
	bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}

	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}

void USpellMenuWidgetController::BroadcastSpellGlobeSelected()
{
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButton(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
	FString Description;
	FString NextLevelDescription;
	const bool bHasAbilityDescription = GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);

	if (!bHasAbilityDescription && AbilityInfo && SelectedAbility.Ability.IsValid() && !SelectedAbility.Ability.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		Description = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).LevelRequirement);
	}

	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}
