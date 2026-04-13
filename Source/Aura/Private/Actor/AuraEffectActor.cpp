// CopyrightHATO


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect>& GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC)) return;

	check(GameplayEffectClass)
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContext);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	const bool bIsInfinite = EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if (bIsInfinite && InfinityEffectRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::ApplyEffectsForPolicy(AActor* TargetActor, EEffectApplicationPolicy PolicyToMatch)
{
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

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	ApplyEffectsForPolicy(TargetActor, EEffectApplicationPolicy::ApplyOnOverlap);
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	ApplyEffectsForPolicy(TargetActor, EEffectApplicationPolicy::ApplyOnEndOverlap);

	if (InfinityEffectRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
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
