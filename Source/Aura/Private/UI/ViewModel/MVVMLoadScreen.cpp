// CopyrightHATO


#include "UI/ViewModel/MVVMLoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVMLoadSlot.h"

void UMVVMLoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);

	LoadSlot_1 = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);

	LoadSlot_2 = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVMLoadSlot* UMVVMLoadScreen::GetLoadSlotByIndex(int32 Index)
{
	if (UMVVMLoadSlot** LoadSlot = LoadSlots.Find(Index))
	{
		return *LoadSlot;
	}

	return nullptr;
}

void UMVVMLoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode) return;

	UMVVMLoadSlot** LoadSlot = LoadSlots.Find(Slot);
	if (!LoadSlot || !IsValid(*LoadSlot)) return;

	(*LoadSlot)->SetPlayerName(EnteredName);
	(*LoadSlot)->SetMapName(AuraGameMode->DefaultMapName);
	(*LoadSlot)->SetPlayerLevel(1);
	(*LoadSlot)->SlotStatus = Taken;
	(*LoadSlot)->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;


	AuraGameMode->SaveSlotData(*LoadSlot, Slot);

	(*LoadSlot)->InitializeSlot();

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	if (!AuraGameInstance) return;

	AuraGameInstance->LoadSlotName = (*LoadSlot)->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = Slot;
	AuraGameInstance->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
}

void UMVVMLoadScreen::NewGameButtonPressed(int32 Slot)
{
	if (UMVVMLoadSlot** LoadSlot = LoadSlots.Find(Slot))
	{
		if (IsValid(*LoadSlot))
		{
			(*LoadSlot)->SetWidgetSwitcherIndex.Broadcast(1);
		}
	}
}

void UMVVMLoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	UMVVMLoadSlot** SelectedLoadSlot = LoadSlots.Find(Slot);
	if (!SelectedLoadSlot || !IsValid(*SelectedLoadSlot)) return;

	OnSlotSelected.Broadcast();
	for (const TTuple<int32, UMVVMLoadSlot*> LoadSlot : LoadSlots)
	{
		if (!IsValid(LoadSlot.Value)) continue;

		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}

	SelectedSlot = *SelectedLoadSlot;
}

void UMVVMLoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AAuraGameModeBase::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
		SelectedSlot = nullptr;
	}
}

void UMVVMLoadScreen::PlayButtonPressed(int32 Slot)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode || !IsValid(SelectedSlot)) return;

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	if (!AuraGameInstance) return;

	UMVVMLoadSlot** LoadSlot = LoadSlots.Find(Slot);
	if (!LoadSlot || !IsValid(*LoadSlot)) return;

	AuraGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotName = SelectedSlot->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;

	AuraGameMode->TravelToMap(*LoadSlot);
}

void UMVVMLoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode) return;

	for (const TTuple<int32, UMVVMLoadSlot*> Slot : LoadSlots)
	{
		if (!IsValid(Slot.Value)) continue;

		ULoadScreenSaveGame* SaveObject = AuraGameMode->GetLoadScreenSaveGame(Slot.Value->LoadSlotName, Slot.Key);
		if (!SaveObject) continue;

		const FString PlayerName = SaveObject->PlayerName;
		const FString MapName = SaveObject->MapName;
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SlotStatus;

		Slot.Value->SlotStatus = SaveSlotStatus;
		Slot.Value->SetPlayerName(PlayerName);
		Slot.Value->SetMapName(MapName);
		Slot.Value->InitializeSlot();

		Slot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		Slot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}
