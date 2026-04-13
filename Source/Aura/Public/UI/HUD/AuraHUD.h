// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UAuraUserWidget;
class UOverlayWidgetController;
class UAttributeWidgetMenuController;
struct FWidgetControllerParams;


/**
 *
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeWidgetMenuController* GetAttributeWidgetMenuController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:

private:

	template<typename T>
	T* GetOrCreateWidgetController(TObjectPtr<T>& WidgetController, TSubclassOf<T> WidgetControllerClass, const FWidgetControllerParams& WCParams)
	{
		if (WidgetController == nullptr)
		{
			WidgetController = NewObject<T>(this, WidgetControllerClass);
			WidgetController->SetWidgetControllerParms(WCParams);
			WidgetController->BindCallbacksToDependencies();
		}
		return WidgetController;
	}

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeWidgetMenuController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeWidgetMenuController> AttributeMenuWidgetControllerClass;
};
