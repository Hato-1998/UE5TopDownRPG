// CopyrightHATO


#include "UI/Widget/AttributeWidgetMenuController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UAttributeWidgetMenuController::BroadcastInitialValues()
{
	check(AttributeMetaData);

	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		BroadcastAttributeMetaData(GetAuraAS(), Pair.Key, Pair.Value);
	}

	if (GetAuraPS())
	{
		OnAttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
		OnSpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
	}
}

void UAttributeWidgetMenuController::BindCallbacksToDependencies()
{
	for (auto& Pair :  GetAuraAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda(
			[this, Tag = Pair.Key, Attribute = Pair.Value](const FOnAttributeChangeData&)
			{
				BroadcastAttributeMetaData( GetAuraAS(), Tag, Attribute);
			}
		);
	}

	if (GetAuraPS())
	{
		GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
			[this](int32 NewAttributePoints)
			{
				OnAttributePointsChangedDelegate.Broadcast(NewAttributePoints);
			});

		GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
			[this](int32 NewSpellPoints)
			{
				OnSpellPointsChangedDelegate.Broadcast(NewSpellPoints);
			});
	}
}

void UAttributeWidgetMenuController::UpgradeAttributePoints(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttributePoints(AttributeTag);
}

void UAttributeWidgetMenuController::BroadcastAttributeMetaData(
	const UAuraAttributeSet* AS, const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeMetaData->FindAttributeInfoForTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AS);
	AttributeInfoDelegate.Broadcast(Info);
}
