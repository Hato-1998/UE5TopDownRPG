// CopyrightHATO


#include "UI/Widget/AttributeWidgetMenuController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAttributeWidgetMenuController::BroadcastInitialValues()
{
}

void UAttributeWidgetMenuController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(
		FAuraGameplayTags::Get().Attribute_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();

	AttributeInfoDelegate.Broadcast(Info);
}
