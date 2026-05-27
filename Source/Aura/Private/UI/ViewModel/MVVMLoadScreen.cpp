// CopyrightHATO


#include "UI/ViewModel/MVVMLoadScreen.h"

#include "Aura/AuraLogChannels.h"
#include "Game/AuraSaveDefaults.h"
#include "Game/AuraSaveGameSubsystem.h"
#include "UI/ViewModel/MVVMLoadSlot.h"

namespace AuraLoadScreenHelpers
{
	static UAuraSaveGameSubsystem* GetSaveSubsystem(const UObject* WorldContextObject)
	{
		if (!WorldContextObject)
		{
			return nullptr;
		}

		if (const UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				return GameInstance->GetSubsystem<UAuraSaveGameSubsystem>();
			}
		}

		return nullptr;
	}
}

void UMVVMLoadScreen::InitializeLoadSlots()
{
	LoadSlots.Empty();

	for (int32 SlotIndex = 0; SlotIndex < AuraSaveDefaults::MaxLoadSlots; ++SlotIndex)
	{
		UMVVMLoadSlot* LoadSlot = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
		LoadSlot->LoadSlotName = AuraSaveDefaults::GetSlotNameForIndex(SlotIndex);
		LoadSlot->SlotIndex = SlotIndex;
		LoadSlots.Add(SlotIndex, LoadSlot);

		switch (SlotIndex)
		{
		case 0:
			LoadSlot_0 = LoadSlot;
			break;
		case 1:
			LoadSlot_1 = LoadSlot;
			break;
		case 2:
			LoadSlot_2 = LoadSlot;
			break;
		default:
			break;
		}
	}
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
	UMVVMLoadSlot** LoadSlot = LoadSlots.Find(Slot);
	if (!LoadSlot || !IsValid(*LoadSlot)) return;

	if (UAuraSaveGameSubsystem* SaveSubsystem = AuraLoadScreenHelpers::GetSaveSubsystem(this))
	{
		SaveSubsystem->CreateNewLoadSlot(*LoadSlot, Slot, EnteredName);
	}
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
		if (UAuraSaveGameSubsystem* SaveSubsystem = AuraLoadScreenHelpers::GetSaveSubsystem(this))
		{
			SaveSubsystem->DeleteLoadSlot(SelectedSlot);
		}
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
		SelectedSlot = nullptr;
	}
}

void UMVVMLoadScreen::PlayButtonPressed(int32 Slot)
{
	UMVVMLoadSlot* SlotToPlay = SelectedSlot;
	if (!IsValid(SlotToPlay))
	{
		SlotToPlay = GetLoadSlotByIndex(Slot);
	}

	if (!IsValid(SlotToPlay))
	{
		UE_LOG(LogAura, Warning, TEXT("PlayButtonPressed ignored: no load slot selected"))
		return;
	}

	if (UAuraSaveGameSubsystem* SaveSubsystem = AuraLoadScreenHelpers::GetSaveSubsystem(this))
	{
		SaveSubsystem->TravelToLoadSlot(SlotToPlay);
	}
}

void UMVVMLoadScreen::LoadData()
{
	UAuraSaveGameSubsystem* SaveSubsystem = AuraLoadScreenHelpers::GetSaveSubsystem(this);
	if (!SaveSubsystem) return;

	for (const TTuple<int32, UMVVMLoadSlot*> Slot : LoadSlots)
	{
		if (!IsValid(Slot.Value)) continue;

		SaveSubsystem->LoadSlotData(Slot.Value, Slot.Key);
	}
}
