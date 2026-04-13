// CopyrightHATO


#include "UI/Widget/AttributeWidgetMenuController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAttributeWidgetMenuController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeMetaData);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeMetaData(AS, Pair.Key, Pair.Value);
	}
}

void UAttributeWidgetMenuController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);
	check(AS);

	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda(
			[this, AS, Tag = Pair.Key, Attribute = Pair.Value](const FOnAttributeChangeData&)
			{
				BroadcastAttributeMetaData(AS, Tag, Attribute);
			}
		);
	}
}

void UAttributeWidgetMenuController::BroadcastAttributeMetaData(
	const UAuraAttributeSet* AS, const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeMetaData->FindAttributeInfoForTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AS);
	AttributeInfoDelegate.Broadcast(Info);
}
