// CopyrightHATO


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	// 바인딩은 캐릭터의 InitAbilityActorInfo에서 BindToASC()로 주입한다.
}

void UPassiveNiagaraComponent::BindToASC(UAbilitySystemComponent* InASC)
{
	if (BoundASC.IsValid())
	{
		if (BoundHandle.IsValid())
		{
			BoundASC->ActivatePassiveAbility.Remove(BoundHandle);
		}
		if (StartupAbilitiesHandle.IsValid())
		{
			BoundASC->AbilitiesGivenDelegate.Remove(StartupAbilitiesHandle);
		}
	}
	BoundASC = nullptr;
	BoundHandle.Reset();
	StartupAbilitiesHandle.Reset();

	if (UAuraAbilitySystemComponent* Aura = Cast<UAuraAbilitySystemComponent>(InASC))
	{
		BoundHandle = Aura->ActivatePassiveAbility.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		BoundASC = Aura;

		if (Aura->bStartupAbilitiesGiven)
		{
			if (Aura->GetStatusFromAbilityTag(PassiveSpellTag) == FAuraGameplayTags::Get().Abilities_Status_Equipped)
			{
				Activate();
			}
		}
		else
		{
			StartupAbilitiesHandle = Aura->AbilitiesGivenDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnStartupAbilitiesGiven);
		}
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::OnStartupAbilitiesGiven()
{
	if (BoundASC.IsValid())
	{
		if (BoundASC->GetStatusFromAbilityTag(PassiveSpellTag) == FAuraGameplayTags::Get().Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
