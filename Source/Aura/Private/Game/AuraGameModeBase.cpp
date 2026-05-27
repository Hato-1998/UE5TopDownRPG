// CopyrightHATO


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Game/AuraGameInstance.h"
#include "GameFramework/PlayerStart.h"

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	APlayerStart* FallbackPlayerStart = nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;
		if (!IsValid(PlayerStart))
		{
			continue;
		}

		if (!FallbackPlayerStart)
		{
			FallbackPlayerStart = PlayerStart;
		}

		if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
		{
			return PlayerStart;
		}
	}

	if (FallbackPlayerStart)
	{
		return FallbackPlayerStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
