// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraWidgetController.h"
#include "AttributeWidgetMenuController.generated.h"

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeWidgetMenuController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
};
