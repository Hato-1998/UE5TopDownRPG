// CopyrightHATO


#include "UI/Widget/AttributeWidgetMenuController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UAttributeWidgetMenuController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeMetaData);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeMetaData(AS, Pair.Key, Pair.Value);
	}

	if (const AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(PlayerState))
	{
		OnAttributePointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
		OnSpellPointsChangedDelegate.Broadcast(AuraPlayerState->GetSpellPoints());
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

	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(PlayerState))
	{
		AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda(
			[this](int32 NewAttributePoints)
			{
				OnAttributePointsChangedDelegate.Broadcast(NewAttributePoints);
			});

		AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda(
			[this](int32 NewSpellPoints)
			{
				OnSpellPointsChangedDelegate.Broadcast(NewSpellPoints);
			});
	}
}

void UAttributeWidgetMenuController::UpgradeAttributePoints(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpgradeAttributePoints(AttributeTag);
}

void UAttributeWidgetMenuController::BroadcastAttributeMetaData(
	const UAuraAttributeSet* AS, const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeMetaData->FindAttributeInfoForTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AS);
	AttributeInfoDelegate.Broadcast(Info);
}
