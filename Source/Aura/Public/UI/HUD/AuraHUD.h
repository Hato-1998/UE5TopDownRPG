// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class USpellMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
class UAuraUserWidget;
class UAuraWidgetController;
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
	USpellMenuWidgetController* GetSpellWidgetMenuController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:

private:

	/**
	 * 통합 캐싱: 동일 컨트롤러 클래스는 한 번만 NewObject. 신규 컨트롤러 타입 추가 시
	 * wrapper 메서드 1줄 + 클래스 변수 1개만 늘어남 (D1 일원화).
	 */
	template<typename T>
	T* GetOrCreateWidgetController(TSubclassOf<T> WidgetControllerClass, const FWidgetControllerParams& WCParams)
	{
		if (!WidgetControllerClass) return nullptr;
		if (TObjectPtr<UAuraWidgetController>* Found = WidgetControllers.Find(WidgetControllerClass))
		{
			return Cast<T>(Found->Get());
		}
		T* NewController = NewObject<T>(this, WidgetControllerClass);
		NewController->SetWidgetControllerParms(WCParams);
		NewController->BindCallbacksToDependencies();
		WidgetControllers.Add(WidgetControllerClass, NewController);
		return NewController;
	}

	/** 컨트롤러 인스턴스 캐시. Key=class, Value=instance. UObject GC 보호용 UPROPERTY. */
	UPROPERTY()
	TMap<TSubclassOf<UAuraWidgetController>, TObjectPtr<UAuraWidgetController>> WidgetControllers;

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeWidgetMenuController> AttributeMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
};
