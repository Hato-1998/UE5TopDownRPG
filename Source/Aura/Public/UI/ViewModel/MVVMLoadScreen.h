// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Templates/SubclassOf.h"
#include "MVVMLoadScreen.generated.h"

class UMVVMLoadSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

UCLASS()
class AURA_API UMVVMLoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()


public:
	void InitializeLoadSlots();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected OnSlotSelected;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVMLoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVMLoadSlot* GetLoadSlotByIndex(int32 Index);

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed(int32 Slot);

	void LoadData();

private:

	UPROPERTY()
	TMap<int32, UMVVMLoadSlot*> LoadSlots;

	UPROPERTY()
	TObjectPtr<UMVVMLoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVMLoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVMLoadSlot> LoadSlot_2;

	UMVVMLoadSlot* SelectedSlot;

};
