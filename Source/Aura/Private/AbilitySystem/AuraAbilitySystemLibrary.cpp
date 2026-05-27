// CopyrightHATO


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Engine/OverlapResult.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Game/AuraGameDataSubsystem.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/PlayerInterface.h"
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
	if (!AuraPlayerState) return false;

	UAbilitySystemComponent* ASC = AuraPlayerState->GetAbilitySystemComponent();
	UAttributeSet* AS = AuraPlayerState->GetAttributeSet();
	if (!ASC || !AS) return false;

	OutParams = FWidgetControllerParams(PlayerController, AuraPlayerState, ASC, AS);
	return true;
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParams(FDamageEffectParams& DamageEffectParams, bool bIsRadial,
	float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	 DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	 DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams,
	FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();

	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}

}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams,
	FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();

	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* ASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = ASC;
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

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellWidgetMenuController(const UObject* WorldContextObject)
{
	FWidgetControllerParams Params;
	AAuraHUD* AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, Params, AuraHUD))
	{
		return AuraHUD->GetSpellWidgetMenuController(Params);
	}
	return nullptr;
}

namespace AuraASLibraryHelpers
{
	static UAuraGameDataSubsystem* GetGameDataSubsystem(const UObject* WorldContextObject)
	{
		if (!GEngine || !WorldContextObject)
		{
			return nullptr;
		}

		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				return GameInstance->GetSubsystem<UAuraGameDataSubsystem>();
			}
		}

		return nullptr;
	}

	/** GE 클래스 1개를 ASC에 적용. Spec 생성·소스 오브젝트 부착·적용 패턴 통합. */
	static void ApplyDefaultEffect(UAbilitySystemComponent* ASC, const AActor* AvatarActor, TSubclassOf<UGameplayEffect> GEClass, float Level)
	{
		if (!ASC || !GEClass) return;
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GEClass, Level, Ctx);
		if (!Spec.IsValid()) return;
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo) return;

	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	AuraASLibraryHelpers::ApplyDefaultEffect(ASC, AvatarActor, ClassDefaultInfo.PrimaryAttributes, Level);
	AuraASLibraryHelpers::ApplyDefaultEffect(ASC, AvatarActor, CharacterClassInfo->SecondaryAttributes, Level);
	AuraASLibraryHelpers::ApplyDefaultEffect(ASC, AvatarActor, CharacterClassInfo->VitalAttributes, Level);
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject,
	UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	const AActor* AvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(AvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->PrimaryAttributes,
		1.f,
		EffectContext);

	if (!SpecHandle.IsValid()) return;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attribute_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attribute_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attribute_Primary_Resilience, SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attribute_Primary_Vigor, SaveGame->Vigor);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	AuraASLibraryHelpers::ApplyDefaultEffect(ASC, AvatarActor, CharacterClassInfo->SecondaryAttributes_Infinite, 1.f);
	AuraASLibraryHelpers::ApplyDefaultEffect(ASC, AvatarActor, CharacterClassInfo->VitalAttributes, 1.f);
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	if (!ASC) return;

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo) return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		if (!AbilityClass) continue;
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (!AbilityClass) continue;

		AActor* AvatarActor = ASC->GetAvatarActor();
		if (AvatarActor && AvatarActor->Implements<UCombatInterface>())
		{
			const int32 PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(AvatarActor);
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, PlayerLevel);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (UAuraGameDataSubsystem* GameDataSubsystem = AuraASLibraryHelpers::GetGameDataSubsystem(WorldContextObject))
	{
		return GameDataSubsystem->GetCharacterClassInfo();
	}

	return nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	if (UAuraGameDataSubsystem* GameDataSubsystem = AuraASLibraryHelpers::GetGameDataSubsystem(WorldContextObject))
	{
		return GameDataSubsystem->GetAbilityInfo();
	}

	return nullptr;
}

ULootTiers* UAuraAbilitySystemLibrary::GetLootTiers(const UObject* WorldContextObject)
{
	if (UAuraGameDataSubsystem* GameDataSubsystem = AuraASLibraryHelpers::GetGameDataSubsystem(WorldContextObject))
	{
		return GameDataSubsystem->GetLootTiers();
	}

	return nullptr;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyGameplayEffect(FDamageEffectParams Params)
{
	if (Params.SourceAbilitySystemComponent == nullptr || Params.TargetAbilitySystemComponent == nullptr) return FGameplayEffectContextHandle();
	if (Params.DamageEffectClass == nullptr) return FGameplayEffectContextHandle();

	FGameplayEffectContextHandle EffectContext = Params.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(Params.SourceAbilitySystemComponent->GetAvatarActor());
	SetDeathImpulse(EffectContext, Params.DeathImpulse);
	SetKnockbackForce(EffectContext, Params.KnockbackForce);
	if (Params.bIsRadialDamage)
	{
		SetIsRadialDamage(EffectContext, true);
		SetRadialDamageInnerRadius(EffectContext, Params.RadialDamageInnerRadius);
		SetRadialDamageOuterRadius(EffectContext, Params.RadialDamageOuterRadius);
		SetRadialDamageOrigin(EffectContext, Params.RadialDamageOrigin);
	}

	const FGameplayEffectSpecHandle SpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(
		Params.DamageEffectClass,
		Params.AbilityLevel,
		EffectContext);

	if (!SpecHandle.IsValid()) return FGameplayEffectContextHandle();

	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Params.DamageType, Params.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Chance, Params.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Damage, Params.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Duration, Params.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Frequency, Params.DebuffFrequency);

	Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	return EffectContext;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}

	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bIsCriticalHit);
	}
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsDebuffDamage();
	}
	return false;
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraEffectContext->GetDamageType().IsValid())
		{
			return *AuraEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetIsDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDebuffDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsDebuffDamage(bInIsDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& DamageType)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageTypePtr = MakeShared<FGameplayTag>(DamageType);
		AuraEffectContext->SetDamageType(DamageTypePtr);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& Impulse)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(Impulse);
	}
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackForce(InForce);
	}
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bIsRadialDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsRadialDamage(bIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, float InnerRadius)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageInnerRadius(InnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, float OuterRadius)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOuterRadius(OuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& Origin)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOrigin(Origin);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayerWithInRadius(const UObject* WorldContextObject,
                                                          TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                          const FVector& SphereOrigin, AActor* SourceAvatar)
{
	FCollisionQueryParams SphereParams;

	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);

		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* OverlapActor = Overlap.GetActor();
			if (OverlapActor && OverlapActor->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(OverlapActor) == false)
			{
				AActor* Avatar = ICombatInterface::Execute_GetAvatar(OverlapActor);
				if (SourceAvatar && !IsNotFriend(SourceAvatar, Avatar))
				{
					continue;
				}
				OutOverlappingActors.AddUnique(Avatar);
			}
		}
	}


}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets.Append(Actors);
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0) break;

		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor = nullptr;
		int32 ClosestIndex = INDEX_NONE;

		for (int32 i = 0; i < ActorsToCheck.Num(); ++i)
		{
			AActor* CurrentActor = ActorsToCheck[i];
			if (!CurrentActor) continue;

			const double DistanceSq = FVector::DistSquared(CurrentActor->GetActorLocation(), Origin);
			if (DistanceSq < ClosestDistance)
			{
				ClosestDistance = DistanceSq;
				ClosestActor = CurrentActor;
				ClosestIndex = i;
			}
		}

		if (ClosestIndex == INDEX_NONE) break;

		ActorsToCheck.RemoveAtSwap(ClosestIndex);
		OutClosestTargets.Add(ClosestActor);
		++NumTargetsFound;
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	if (!FirstActor || !SecondActor)
	{
		return false;
	}

	// 1. 둘 다 'Player' 태그를 가지고 있는지 확인
	const bool bBothArePlayers = FirstActor->Implements<UPlayerInterface>() && SecondActor->Implements<UPlayerInterface>();

	// 2. 둘 다 'Enemy' 태그를 가지고 있는지 확인
	const bool bBothAreEnemies = FirstActor->Implements<UEnemyInterface>() && SecondActor->Implements<UEnemyInterface>();

	// 3. 둘 다 플레이어이거나 둘 다 적일 때만 아군(false) 반환,
	// 그 외의 경우(바위, 맵 오브젝트 등 태그가 없는 대상 포함)는 모두 아군이 아님(true) 반환
	return !(bBothArePlayers || bBothAreEnemies);
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumRotators)
{
	TArray<FRotator> Rotators;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, FVector::UpVector);

	if (NumRotators > 1 )
	{
		const float DeltaSpread = Spread / (NumRotators - 1);

		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);

			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumVectors)
{
	TArray<FVector> Vectors;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, FVector::UpVector);

	if (NumVectors > 1 )
	{
		const float DeltaSpread = Spread / (NumVectors - 1);

		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);

			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}

	return Vectors;
}
