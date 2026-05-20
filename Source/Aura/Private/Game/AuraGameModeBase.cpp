// CopyrightHATO


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraSaveCustomVersion.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVMLoadSlot.h"

class UAuraGameInstance;

void AAuraGameModeBase::SaveSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex)
{
	if (!LoadSlot)
	{
		UE_LOG(LogAura, Error, TEXT("SaveSlotData failed: LoadSlot is null"))
		return;
	}

	if (!LoadScreenSaveGameClass)
	{
		UE_LOG(LogAura, Error, TEXT("SaveSlotData failed: LoadScreenSaveGameClass is invalid"))
		return;
	}

	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	if (!LoadScreenSaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("SaveSlotData failed: Could not create save game object"))
		return;
	}

	DeleteSlot(LoadSlot->LoadSlotName, SlotIndex);

	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName = DefaultMap.ToSoftObjectPath().GetAssetName();
	LoadScreenSaveGame->SlotStatus = Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
	AsyncSaveDelegate.BindLambda(
		[](const FString& InSlotName, const int32 InUserIndex, bool bSuccess)
		{
			if (!bSuccess)
			{
				UE_LOG(LogAura, Error, TEXT("SaveSlotData async failed: slot %s index %d"), *InSlotName, InUserIndex);
			}
		});
	UGameplayStatics::AsyncSaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex, AsyncSaveDelegate);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetLoadScreenSaveGame(const FString& SlotName, int32 SlotIndex) const
{
	if (!LoadScreenSaveGameClass)
	{
		UE_LOG(LogAura, Error, TEXT("GetLoadScreenSaveGame failed: LoadScreenSaveGameClass is not set"))
		return nullptr;
	}

	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	if (!LoadScreenSaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("GetLoadScreenSaveGame failed: Could not load or create slot %s index %d"), *SlotName, SlotIndex)
	}
	return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

void AAuraGameModeBase::TravelToMap(UMVVMLoadSlot* Slot)
{
	if (!Slot)
	{
		UE_LOG(LogAura, Error, TEXT("TravelToMap failed: Slot is null"))
		return;
	}

	const TSoftObjectPtr<UWorld>* MapToTravel = Maps.Find(Slot->GetMapName());
	if (!MapToTravel)
	{
		UE_LOG(LogAura, Error, TEXT("TravelToMap failed: Map %s is not registered"), *Slot->GetMapName())
		return;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, *MapToTravel);
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	TArray<AActor*> Actors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	if (Actors.Num() > 0)
	{
		AActor* SelectActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectActor = PlayerStart;
					break;
				}
			}
		}

		return SelectActor;
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		UE_LOG(LogAura, Error, TEXT("RetrieveInGameSaveData failed: AuraGameInstance is null"))
		return nullptr;
	}

	const FString SlotName = AuraGameInstance->LoadSlotName;
	const int32 SlotIndex = AuraGameInstance->LoadSlotIndex;

	return GetLoadScreenSaveGame(SlotName, SlotIndex);
}

void AAuraGameModeBase::SaveInGameSaveData(ULoadScreenSaveGame* SaveData)
{
	if (!SaveData)
	{
		UE_LOG(LogAura, Error, TEXT("SaveInGameSaveData failed: SaveData is null"))
		return;
	}

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		UE_LOG(LogAura, Error, TEXT("SaveInGameSaveData failed: AuraGameInstance is null"))
		return;
	}

	const FString SlotName = AuraGameInstance->LoadSlotName;
	const int32 SlotIndex = AuraGameInstance->LoadSlotIndex;
	AuraGameInstance->PlayerStartTag = SaveData->PlayerStartTag;

	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
	AsyncSaveDelegate.BindLambda(
		[](const FString& InSlotName, const int32 InUserIndex, bool bSuccess)
		{
			if (!bSuccess)
			{
				UE_LOG(LogAura, Error, TEXT("SaveInGameSaveData async failed: slot %s index %d"), *InSlotName, InUserIndex);
			}
		});
	UGameplayStatics::AsyncSaveGameToSlot(SaveData, SlotName, SlotIndex, AsyncSaveDelegate);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	if (!World)
	{
		UE_LOG(LogAura, Error, TEXT("SaveWorldState failed: World is null"))
		return;
	}

	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		UE_LOG(LogAura, Error, TEXT("SaveWorldState failed: AuraGameInstance is null"))
		return;
	}

	if (ULoadScreenSaveGame* SaveGame = GetLoadScreenSaveGame(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
		}

		FSavedMap* SavedMap = SaveGame->SavedMaps.FindByPredicate([&WorldName](const FSavedMap& Map)
		{
			return Map.MapAssetName == WorldName;
		});

		if (!SavedMap)
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			const int32 NewMapIndex = SaveGame->SavedMaps.Add(NewSavedMap);
			SavedMap = &SaveGame->SavedMaps[NewMapIndex];
		}

		SavedMap->SavedActors.Empty();

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			FMemoryWriter MemoryWriter(SavedActor.Byte);

			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			Archive.UsingCustomVersion(FAuraSaveCustomVersion::GUID);

			Actor->Serialize(Archive);

			SavedMap->SavedActors.AddUnique(SavedActor);
		}

		FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
		AsyncSaveDelegate.BindLambda(
			[](const FString& InSlotName, const int32 InUserIndex, bool bSuccess)
			{
				if (!bSuccess)
				{
					UE_LOG(LogAura, Error, TEXT("SaveWorldState async failed: slot %s index %d"), *InSlotName, InUserIndex);
				}
			});
		UGameplayStatics::AsyncSaveGameToSlot(SaveGame, AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex, AsyncSaveDelegate);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	if (!World)
	{
		UE_LOG(LogAura, Error, TEXT("LoadWorldState failed: World is null"))
		return;
	}

	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		UE_LOG(LogAura, Error, TEXT("LoadWorldState failed: AuraGameInstance is null"))
		return;
	}

	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("Failed to load slot"))
			return;
		}

		const FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			for (const FSavedActor& SavedActor : SavedMap.SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}

					FMemoryReader MemoryReader(SavedActor.Byte);

					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					Archive.UsingCustomVersion(FAuraSaveCustomVersion::GUID);
					Actor->Serialize(Archive);

					ISaveInterface::Execute_LoadActor(Actor);

					break;
				}
			}
		}
	}

}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

void AAuraGameModeBase::PlayerDied(ACharacter* DeadCharacter)
{
	ULoadScreenSaveGame* SaveGame = RetrieveInGameSaveData();
	if (!SaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("PlayerDied failed: SaveGame is null"))
		return;
	}

	const TSoftObjectPtr<UWorld>* MapToTravel = Maps.Find(GetMapNameFromMapAssetName(SaveGame->MapAssetName));
	if (!MapToTravel || MapToTravel->IsNull())
	{
		UE_LOG(LogAura, Error, TEXT("PlayerDied failed: Map %s is not registered"), *SaveGame->MapAssetName)
		return;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(DeadCharacter, *MapToTravel);
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
