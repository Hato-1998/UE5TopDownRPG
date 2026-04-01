// CopyrightHATO


#include "UI/Widget/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParms(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValves()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}
