// CopyrightHATO


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Game/AuraGameModeBase.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	FCharacterClassDefaultInfo ClassDefaultInfo = AuraGameMode->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributeContext = ASC->MakeEffectContext();
	PrimaryAttributeContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimarySpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributeContext);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributeContext = ASC->MakeEffectContext();
	SecondaryAttributeContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributeContext);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributeContext = ASC->MakeEffectContext();
	VitalAttributeContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level,VitalAttributeContext);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}
