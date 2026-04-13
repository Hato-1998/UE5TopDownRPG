// CopyrightHATO


#include "UI/Widget/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParms(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	// 서브클래스에서 오버라이드하여 초기 Attribute 값을 위젯에 브로드캐스트
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
	// 서브클래스에서 오버라이드하여 Attribute 변경 콜백을 바인딩
}
