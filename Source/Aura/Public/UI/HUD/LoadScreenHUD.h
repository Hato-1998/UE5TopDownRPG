// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class UMVVMLoadScreen;
class ULoadScreenWidget;

/**
 *
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoadScreenWidget> LoadScreenWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVMLoadScreen> LoadScreenViewModelClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVMLoadScreen> LoadScreenViewModel;
protected:
	virtual void BeginPlay() override;
};
