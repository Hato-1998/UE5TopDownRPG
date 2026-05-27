// CopyrightHATO


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/EnemyInterface.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();
}


void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
	CalculatedLocation = InitialLocation;
}


void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect>& GameplayEffectClass)
{
	if (!CanApplyEffectsToTarget(TargetActor)) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC)) return;

	if (!GameplayEffectClass) return;
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContext);
	if (!EffectSpec.IsValid()) return;

	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	const bool bIsInfinite = EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if (bIsInfinite && InfinityEffectRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	if (bDestroyOnEffectApplication && EffectSpec.Data.Get()->Def.Get()->DurationPolicy != EGameplayEffectDurationType::Infinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::ApplyEffectsForPolicy(AActor* TargetActor, EEffectApplicationPolicy PolicyToMatch)
{
	if (!CanApplyEffectsToTarget(TargetActor)) return;

	if (InstantEffectApplicationPolicy == PolicyToMatch)
	{
		ApplyEffectToTarget(TargetActor, InstantGamePlayEffectClass);
	}

	if (DurationEffectApplicationPolicy == PolicyToMatch)
	{
		ApplyEffectToTarget(TargetActor, DurationGamePlayEffectClass);
	}

	if (InfinityEffectApplicationPolicy == PolicyToMatch)
	{
		ApplyEffectToTarget(TargetActor, InfinityGamePlayEffectClass);
	}
}

bool AAuraEffectActor::CanApplyEffectsToTarget(const AActor* TargetActor) const
{
	if (!IsValid(TargetActor)) return false;
	return bApplyEffectsToEnemies || !TargetActor->Implements<UEnemyInterface>();
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	ApplyEffectsForPolicy(TargetActor, EEffectApplicationPolicy::ApplyOnOverlap);
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (!CanApplyEffectsToTarget(TargetActor)) return;

	ApplyEffectsForPolicy(TargetActor, EEffectApplicationPolicy::ApplyOnEndOverlap);

	if (InfinityEffectRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (const TTuple<FActiveGameplayEffectHandle, TWeakObjectPtr<UAbilitySystemComponent>>& HandlePair : ActiveEffectHandles)
		{
			if (!HandlePair.Value.IsValid())
			{
				HandlesToRemove.Add(HandlePair.Key);
				continue;
			}

			if (TargetASC == HandlePair.Value.Get())
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
