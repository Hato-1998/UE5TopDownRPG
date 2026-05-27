// CopyrightHATO


#include "Game/AuraSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraSaveCustomVersion.h"
#include "Game/AuraSaveDefaults.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVMLoadSlot.h"

bool UAuraSaveGameSubsystem::CreateNewLoadSlot(UMVVMLoadSlot* LoadSlot, int32 SlotIndex, const FString& PlayerName) const
{
	if (!LoadSlot)
	{
		UE_LOG(LogAura, Error, TEXT("CreateNewLoadSlot failed: LoadSlot is null"))
		return false;
	}

	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("CreateNewLoadSlot failed: AuraGameMode is unavailable"))
		return false;
	}

	LoadSlot->SetPlayerName(PlayerName);
	LoadSlot->SetMapName(AuraGameMode->DefaultMapName);
	LoadSlot->SetPlayerLevel(1);
	LoadSlot->SlotStatus = Taken;
	LoadSlot->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;

	SaveSlotData(LoadSlot, SlotIndex, AuraGameMode->LoadScreenSaveGameClass, AuraGameMode->DefaultMapName,
		AuraGameMode->DefaultMap, AuraGameMode->DefaultPlayerStartTag);

	LoadSlot->InitializeSlot();
	return SetCurrentLoadSlot(LoadSlot);
}

bool UAuraSaveGameSubsystem::LoadSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex) const
{
	if (!LoadSlot)
	{
		return false;
	}

	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("LoadSlotData failed: AuraGameMode is unavailable"))
		return false;
	}

	ULoadScreenSaveGame* SaveObject = GetLoadScreenSaveGame(LoadSlot->LoadSlotName, SlotIndex, AuraGameMode->LoadScreenSaveGameClass);
	if (!SaveObject)
	{
		return false;
	}

	const TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SlotStatus;
	LoadSlot->SlotStatus = SaveSlotStatus;
	LoadSlot->SetPlayerName(SaveSlotStatus == Taken ? SaveObject->PlayerName : FString());
	LoadSlot->SetMapName(SaveSlotStatus == Taken ? SaveObject->MapName : FString());
	LoadSlot->PlayerStartTag = SaveObject->PlayerStartTag;
	LoadSlot->SetPlayerLevel(SaveSlotStatus == Taken ? SaveObject->PlayerLevel : 1);
	LoadSlot->InitializeSlot();
	return true;
}

void UAuraSaveGameSubsystem::DeleteLoadSlot(UMVVMLoadSlot* LoadSlot) const
{
	if (!LoadSlot)
	{
		return;
	}

	DeleteSlot(LoadSlot->LoadSlotName, LoadSlot->SlotIndex);
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance()))
	{
		if (AuraGameInstance->LoadSlotName == LoadSlot->LoadSlotName && AuraGameInstance->LoadSlotIndex == LoadSlot->SlotIndex)
		{
			AuraGameInstance->LoadSlotName.Reset();
			AuraGameInstance->LoadSlotIndex = 0;
			AuraGameInstance->PlayerStartTag = FName(AuraSaveDefaults::DefaultPlayerStartTag);
		}
	}

	LoadSlot->SlotStatus = Vacant;
	LoadSlot->SetPlayerName(FString());
	LoadSlot->SetMapName(FString());
	LoadSlot->SetPlayerLevel(1);
	LoadSlot->PlayerStartTag = FName();
	LoadSlot->InitializeSlot();
}

bool UAuraSaveGameSubsystem::TravelToLoadSlot(UMVVMLoadSlot* LoadSlot) const
{
	if (!LoadSlot)
	{
		UE_LOG(LogAura, Warning, TEXT("TravelToLoadSlot ignored: LoadSlot is null"))
		return false;
	}

	if (LoadSlot->SlotStatus != Taken)
	{
		UE_LOG(LogAura, Warning, TEXT("TravelToLoadSlot ignored: selected slot %s is not taken"), *LoadSlot->LoadSlotName)
		return false;
	}

	if (LoadSlot->GetMapName().IsEmpty() || LoadSlot->GetMapName() == AuraSaveDefaults::DefaultMapName)
	{
		UE_LOG(LogAura, Error, TEXT("TravelToLoadSlot failed: selected slot %s has invalid map name '%s'"),
			*LoadSlot->LoadSlotName, *LoadSlot->GetMapName())
		return false;
	}

	AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("TravelToLoadSlot failed: AuraGameMode is unavailable"))
		return false;
	}

	if (!SetCurrentLoadSlot(LoadSlot))
	{
		return false;
	}

	const TSoftObjectPtr<UWorld>* MapToTravel = AuraGameMode->Maps.Find(LoadSlot->GetMapName());
	if (!MapToTravel || MapToTravel->IsNull())
	{
		UE_LOG(LogAura, Error, TEXT("TravelToLoadSlot failed: Map %s is not registered"), *LoadSlot->GetMapName())
		return false;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot, *MapToTravel);
	return true;
}

void UAuraSaveGameSubsystem::SaveSlotData(UMVVMLoadSlot* LoadSlot, int32 SlotIndex, TSubclassOf<ULoadScreenSaveGame> SaveGameClass,
	const FString& DefaultMapName, const TSoftObjectPtr<UWorld>& DefaultMap, FName DefaultPlayerStartTag) const
{
	if (!LoadSlot)
	{
		UE_LOG(LogAura, Error, TEXT("SaveSlotData failed: LoadSlot is null"))
		return;
	}

	if (!SaveGameClass)
	{
		UE_LOG(LogAura, Error, TEXT("SaveSlotData failed: SaveGameClass is invalid"))
		return;
	}

	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	if (!LoadScreenSaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("SaveSlotData failed: Could not create save game object"))
		return;
	}

	DeleteSlot(LoadSlot->LoadSlotName, SlotIndex);

	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName().IsEmpty() ? DefaultMapName : LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName = DefaultMap.ToSoftObjectPath().GetAssetName();
	LoadScreenSaveGame->SlotStatus = Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag.IsNone() ? DefaultPlayerStartTag : LoadSlot->PlayerStartTag;

	SaveToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex, TEXT("SaveSlotData"));
}

ULoadScreenSaveGame* UAuraSaveGameSubsystem::GetLoadScreenSaveGame(const FString& SlotName, int32 SlotIndex,
	TSubclassOf<ULoadScreenSaveGame> SaveGameClass) const
{
	if (!SaveGameClass)
	{
		UE_LOG(LogAura, Error, TEXT("GetLoadScreenSaveGame failed: SaveGameClass is not set"))
		return nullptr;
	}

	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
		if (!SaveGameObject)
		{
			UE_LOG(LogAura, Warning, TEXT("GetLoadScreenSaveGame: Failed to load slot %s index %d. Creating a new save object."), *SlotName, SlotIndex);
		}
	}

	if (!SaveGameObject)
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
	}

	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	if (!LoadScreenSaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("GetLoadScreenSaveGame failed: Could not load or create slot %s index %d"), *SlotName, SlotIndex)
		return nullptr;
	}

	LoadScreenSaveGame->SlotName = SlotName;
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	return LoadScreenSaveGame;
}

void UAuraSaveGameSubsystem::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* UAuraSaveGameSubsystem::RetrieveInGameSaveData() const
{
	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("RetrieveInGameSaveData failed: AuraGameMode is unavailable"))
		return nullptr;
	}

	return RetrieveInGameSaveData(AuraGameMode->LoadScreenSaveGameClass);
}

ULoadScreenSaveGame* UAuraSaveGameSubsystem::RetrieveInGameSaveData(TSubclassOf<ULoadScreenSaveGame> SaveGameClass) const
{
	FString SlotName;
	int32 SlotIndex = 0;
	if (!GetCurrentSlot(SlotName, SlotIndex))
	{
		UE_LOG(LogAura, Error, TEXT("RetrieveInGameSaveData failed: current slot is not initialized"))
		return nullptr;
	}

	return GetLoadScreenSaveGame(SlotName, SlotIndex, SaveGameClass);
}

void UAuraSaveGameSubsystem::SaveInGameSaveData(ULoadScreenSaveGame* SaveData) const
{
	if (!SaveData)
	{
		UE_LOG(LogAura, Error, TEXT("SaveInGameSaveData failed: SaveData is null"))
		return;
	}

	FString SlotName;
	int32 SlotIndex = 0;
	if (!GetCurrentSlot(SlotName, SlotIndex))
	{
		UE_LOG(LogAura, Error, TEXT("SaveInGameSaveData failed: current slot is not initialized"))
		return;
	}

	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance()))
	{
		AuraGameInstance->PlayerStartTag = SaveData->PlayerStartTag;
	}

	SaveToSlot(SaveData, SlotName, SlotIndex, TEXT("SaveInGameSaveData"));
}

void UAuraSaveGameSubsystem::SavePlayerProgress(AActor* PlayerActor, FName CheckpointTag) const
{
	AAuraCharacterBase* AuraCharacter = Cast<AAuraCharacterBase>(PlayerActor);
	if (!AuraCharacter)
	{
		UE_LOG(LogAura, Error, TEXT("SavePlayerProgress failed: PlayerActor is not an AuraCharacterBase"))
		return;
	}

	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("SavePlayerProgress failed: AuraGameMode is unavailable"))
		return;
	}

	ULoadScreenSaveGame* SaveData = RetrieveInGameSaveData(AuraGameMode->LoadScreenSaveGameClass);
	if (!SaveData)
	{
		UE_LOG(LogAura, Error, TEXT("SavePlayerProgress failed: SaveData is null"))
		return;
	}

	SaveData->PlayerStartTag = CheckpointTag;

	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(AuraCharacter->GetPlayerState()))
	{
		SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
		SaveData->PlayerXP = AuraPlayerState->GetXP();
		SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
		SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
	}

	UAttributeSet* AttributeSet = AuraCharacter->GetAttributeSet();
	if (!AttributeSet)
	{
		UE_LOG(LogAura, Error, TEXT("SavePlayerProgress failed: AttributeSet is null"))
		return;
	}

	SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(AttributeSet);
	SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(AttributeSet);
	SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(AttributeSet);
	SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(AttributeSet);
	SaveData->bFirstTimeLoadIn = false;

	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AuraCharacter->GetAbilitySystemComponent());
	if (!AuraASC)
	{
		UE_LOG(LogAura, Error, TEXT("SavePlayerProgress failed: AuraASC is null"))
		return;
	}

	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(AuraCharacter);
	if (!AbilityInfo)
	{
		UE_LOG(LogAura, Error, TEXT("SavePlayerProgress failed: AbilityInfo is null"))
		return;
	}

	FForEachAbility SaveAbilityDelegate;
	SaveData->SaveAbilities.Empty();
	SaveAbilityDelegate.BindLambda([AuraASC, AbilityInfo, SaveData](const FGameplayAbilitySpec& AbilitySpec)
	{
		const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromSpec(AbilitySpec);
		const FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

		FSavedAbility SavedAbility;
		SavedAbility.GameplayAbility = Info.Ability;
		SavedAbility.AbilityLevel = AbilitySpec.Level;
		SavedAbility.AbilitySlot = AuraASC->GetSlotFromAbilityTag(AbilityTag);
		SavedAbility.AbilityStatus = AuraASC->GetStatusFromAbilityTag(AbilityTag);
		SavedAbility.AbilityTag = AbilityTag;
		SavedAbility.AbilityType = Info.AbilityType;

		SaveData->SaveAbilities.AddUnique(SavedAbility);
	});

	AuraASC->ForEachAbility(SaveAbilityDelegate);
	SaveInGameSaveData(SaveData);
}

void UAuraSaveGameSubsystem::HandlePlayerDeath(AActor* DeadActor) const
{
	if (!DeadActor)
	{
		UE_LOG(LogAura, Error, TEXT("HandlePlayerDeath failed: DeadActor is null"))
		return;
	}

	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("HandlePlayerDeath failed: AuraGameMode is unavailable"))
		return;
	}

	ULoadScreenSaveGame* SaveGame = RetrieveInGameSaveData(AuraGameMode->LoadScreenSaveGameClass);
	if (!SaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("HandlePlayerDeath failed: SaveGame is null"))
		return;
	}

	FString MapName;
	if (!FindMapNameFromMapAssetName(AuraGameMode->Maps, SaveGame->MapAssetName, MapName))
	{
		UE_LOG(LogAura, Error, TEXT("HandlePlayerDeath failed: Map asset %s is not registered"), *SaveGame->MapAssetName)
		return;
	}

	const TSoftObjectPtr<UWorld>* MapToTravel = AuraGameMode->Maps.Find(MapName);
	if (!MapToTravel || MapToTravel->IsNull())
	{
		UE_LOG(LogAura, Error, TEXT("HandlePlayerDeath failed: Map %s points to an invalid asset"), *MapName)
		return;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(DeadActor, *MapToTravel);
}

void UAuraSaveGameSubsystem::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("SaveWorldState failed: AuraGameMode is unavailable"))
		return;
	}

	SaveWorldState(World, AuraGameMode->LoadScreenSaveGameClass, AuraGameMode->Maps, DestinationMapAssetName);
}

void UAuraSaveGameSubsystem::SaveWorldState(UWorld* World, TSubclassOf<ULoadScreenSaveGame> SaveGameClass,
	const TMap<FString, TSoftObjectPtr<UWorld>>& Maps, const FString& DestinationMapAssetName) const
{
	if (!World)
	{
		UE_LOG(LogAura, Error, TEXT("SaveWorldState failed: World is null"))
		return;
	}

	FString SlotName;
	int32 SlotIndex = 0;
	if (!GetCurrentSlot(SlotName, SlotIndex))
	{
		UE_LOG(LogAura, Error, TEXT("SaveWorldState failed: current slot is not initialized"))
		return;
	}

	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	if (ULoadScreenSaveGame* SaveGame = GetLoadScreenSaveGame(SlotName, SlotIndex, SaveGameClass))
	{
		if (!DestinationMapAssetName.IsEmpty())
		{
			FString DestinationMapName;
			if (!FindMapNameFromMapAssetName(Maps, DestinationMapAssetName, DestinationMapName))
			{
				UE_LOG(LogAura, Error, TEXT("SaveWorldState failed: destination map asset %s is not registered"), *DestinationMapAssetName);
				return;
			}

			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = DestinationMapName;
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
			SavedActor.SaveId = ISaveInterface::Execute_GetSaveId(Actor);
			SavedActor.Transform = Actor->GetTransform();

			FMemoryWriter MemoryWriter(SavedActor.Byte);

			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			Archive.UsingCustomVersion(FAuraSaveCustomVersion::GUID);

			Actor->Serialize(Archive);

			SavedMap->SavedActors.AddUnique(SavedActor);
		}

		SaveToSlot(SaveGame, SlotName, SlotIndex, TEXT("SaveWorldState"));
	}
}

void UAuraSaveGameSubsystem::LoadWorldState(UWorld* World) const
{
	const AAuraGameModeBase* AuraGameMode = GetAuraGameMode();
	if (!AuraGameMode)
	{
		UE_LOG(LogAura, Error, TEXT("LoadWorldState failed: AuraGameMode is unavailable"))
		return;
	}

	LoadWorldState(World, AuraGameMode->LoadScreenSaveGameClass);
}

void UAuraSaveGameSubsystem::LoadWorldState(UWorld* World, TSubclassOf<ULoadScreenSaveGame> SaveGameClass) const
{
	if (!World)
	{
		UE_LOG(LogAura, Error, TEXT("LoadWorldState failed: World is null"))
		return;
	}

	FString SlotName;
	int32 SlotIndex = 0;
	if (!GetCurrentSlot(SlotName, SlotIndex))
	{
		UE_LOG(LogAura, Error, TEXT("LoadWorldState failed: current slot is not initialized"))
		return;
	}

	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = GetLoadScreenSaveGame(SlotName, SlotIndex, SaveGameClass);
		if (!SaveGame)
		{
			UE_LOG(LogAura, Error, TEXT("LoadWorldState failed: Could not load slot %s index %d"), *SlotName, SlotIndex)
			return;
		}

		const FSavedMap* SavedMap = SaveGame->FindSavedMapWithMapName(WorldName);
		if (!SavedMap)
		{
			return;
		}

		TMap<FGuid, const FSavedActor*> SavedActorsById;
		TMap<FName, const FSavedActor*> LegacySavedActorsByName;
		for (const FSavedActor& SavedActor : SavedMap->SavedActors)
		{
			if (SavedActor.SaveId.IsValid())
			{
				SavedActorsById.Add(SavedActor.SaveId, &SavedActor);
			}
			else
			{
				LegacySavedActorsByName.Add(SavedActor.ActorName, &SavedActor);
			}
		}

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			const FSavedActor* SavedActor = nullptr;
			const FGuid ActorSaveId = ISaveInterface::Execute_GetSaveId(Actor);
			if (ActorSaveId.IsValid())
			{
				SavedActor = SavedActorsById.FindRef(ActorSaveId);
			}
			if (!SavedActor)
			{
				SavedActor = LegacySavedActorsByName.FindRef(Actor->GetFName());
			}
			if (!SavedActor) continue;

			if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
			{
				Actor->SetActorTransform(SavedActor->Transform);
			}

			FMemoryReader MemoryReader(SavedActor->Byte);

			FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
			Archive.ArIsSaveGame = true;
			Archive.UsingCustomVersion(FAuraSaveCustomVersion::GUID);
			Actor->Serialize(Archive);

			ISaveInterface::Execute_LoadActor(Actor);
		}
	}
}

bool UAuraSaveGameSubsystem::SaveToSlot(USaveGame* SaveGame, const FString& SlotName, int32 SlotIndex, const TCHAR* Context)
{
	if (!SaveGame)
	{
		UE_LOG(LogAura, Error, TEXT("%s failed: SaveGame is null"), Context);
		return false;
	}

	if (!UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, SlotIndex))
	{
		UE_LOG(LogAura, Error, TEXT("%s failed: slot %s index %d"), Context, *SlotName, SlotIndex);
		return false;
	}

	return true;
}

bool UAuraSaveGameSubsystem::FindMapNameFromMapAssetName(const TMap<FString, TSoftObjectPtr<UWorld>>& Maps, const FString& MapAssetName, FString& OutMapName)
{
	for (const TTuple<FString, TSoftObjectPtr<UWorld>>& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			OutMapName = Map.Key;
			return !OutMapName.IsEmpty();
		}
	}
	return false;
}

AAuraGameModeBase* UAuraSaveGameSubsystem::GetAuraGameMode() const
{
	if (const UWorld* World = GetWorld())
	{
		return World->GetAuthGameMode<AAuraGameModeBase>();
	}

	return nullptr;
}

bool UAuraSaveGameSubsystem::SetCurrentLoadSlot(const UMVVMLoadSlot* LoadSlot) const
{
	if (!LoadSlot)
	{
		return false;
	}

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		UE_LOG(LogAura, Error, TEXT("SetCurrentLoadSlot failed: AuraGameInstance is null"))
		return false;
	}

	AuraGameInstance->PlayerStartTag = LoadSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotName = LoadSlot->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = LoadSlot->SlotIndex;
	return true;
}

bool UAuraSaveGameSubsystem::GetCurrentSlot(FString& OutSlotName, int32& OutSlotIndex) const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		return false;
	}

	OutSlotName = AuraGameInstance->LoadSlotName;
	OutSlotIndex = AuraGameInstance->LoadSlotIndex;
	if (OutSlotName.IsEmpty())
	{
		OutSlotName = AuraSaveDefaults::FallbackSlotName;
		OutSlotIndex = 0;
		AuraGameInstance->LoadSlotName = OutSlotName;
		AuraGameInstance->LoadSlotIndex = OutSlotIndex;
		UE_LOG(LogAura, Warning, TEXT("AuraSaveGameSubsystem: Current slot was empty. Falling back to %s index %d."), *OutSlotName, OutSlotIndex);
	}

	return true;
}
