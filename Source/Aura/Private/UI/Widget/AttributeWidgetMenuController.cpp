// CopyrightHATO


#include "UI/Widget/AttributeWidgetMenuController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerState.h"

void UAttributeWidgetMenuController::BroadcastInitialValues()
{
	if (!AttributeMetaData)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: AttributeMetaData is not set. Attribute menu values were not broadcast."),
			__FUNCTION__);
		return;
	}

	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		BroadcastAttributeMetaData(GetAuraAS(), Pair.Key, Pair.Value);
	}

	if (GetAuraPS())
	{
		OnAttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
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
	}
}

void UAttributeWidgetMenuController::UpgradeAttributePoints(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttributePoints(AttributeTag);
}

void UAttributeWidgetMenuController::BroadcastAttributeMetaData(
	const UAuraAttributeSet* AS, const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	if (!AttributeMetaData)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: AttributeMetaData is not set. Skipping attribute tag %s."),
			__FUNCTION__, *Tag.ToString());
		return;
	}

	FAuraAttributeInfo Info = AttributeMetaData->FindAttributeInfoForTag(Tag);
	if (!Info.AttributeTag.IsValid())
	{
		UE_LOG(LogAura, Warning,
			TEXT("%hs: Missing AttributeInfo metadata for tag %s in %s. UI broadcast skipped."),
			__FUNCTION__,
			*Tag.ToString(),
			*GetNameSafe(AttributeMetaData));
		return;
	}

	Info.AttributeValue = Attribute.GetNumericValue(AS);
	AttributeInfoDelegate.Broadcast(Info);
}
