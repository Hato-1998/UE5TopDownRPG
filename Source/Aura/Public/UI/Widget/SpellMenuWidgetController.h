// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool,
                                             bEquipButtonEnabled, FString, Description, FString, NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignSignature, const FGameplayTag&, AbilityType );

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilities")
	FOnPlayerStatChangedSignature OnSpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilities")
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilities")
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilities")
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilities")
	FSpellGlobeReassignSignature SpellGlobeReassignDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void GlobeDeselected();

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

private:
	static void ShouldEnableButton(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);

	void BroadcastSpellGlobeSelected();

	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints = 0;

	bool bWaitForEquipSelection = false;

	FGameplayTag SelectedSlot;
};
