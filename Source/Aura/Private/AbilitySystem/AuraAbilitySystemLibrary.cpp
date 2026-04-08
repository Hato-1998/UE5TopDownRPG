// CopyrightHATO


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraWidgetController.h"
#include "UI/Widget/OverlayWidgetController.h"
#include "UI/Widget/AttributeWidgetMenuController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutParams, AAuraHUD*& OutHUD)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PlayerController) return false;

	OutHUD = Cast<AAuraHUD>(PlayerController->GetHUD());
	if (!OutHUD) return false;

	AAuraPlayerState* AuraPlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
	UAbilitySystemComponent* ASC = AuraPlayerState->GetAbilitySystemComponent();
	UAttributeSet* AS = AuraPlayerState->GetAttributeSet();

	OutParams = FWidgetControllerParams(PlayerController, AuraPlayerState, ASC, AS);
	return true;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams Params;
	AAuraHUD* AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, Params, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(Params);
	}
	return nullptr;
}

UAttributeWidgetMenuController* UAuraAbilitySystemLibrary::GetAttributeWidgetMenuController(const UObject* WorldContextObject)
{
	FWidgetControllerParams Params;
	AAuraHUD* AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, Params, AuraHUD))
	{
		return AuraHUD->GetAttributeWidgetMenuController(Params);
	}
	return nullptr;
}
