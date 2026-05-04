// CopyrightHATO


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/Widget/OverlayWidgetController.h"
#include "UI/Widget/AttributeWidgetMenuController.h"
#include "UI/Widget/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	return GetOrCreateWidgetController(OverlayWidgetController, OverlayWidgetControllerClass, WCParams);
}

UAttributeWidgetMenuController* AAuraHUD::GetAttributeWidgetMenuController(const FWidgetControllerParams& WCParams)
{
	return GetOrCreateWidgetController(AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, WCParams);
}

USpellMenuWidgetController* AAuraHUD::GetSpellWidgetMenuController(const FWidgetControllerParams& WCParams)
{
	return GetOrCreateWidgetController(SpellMenuWidgetController, SpellMenuWidgetControllerClass, WCParams);
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class Uninitialized"))
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class Uninitialized"))

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParms(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParms);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();

	Widget->AddToViewport();
}
