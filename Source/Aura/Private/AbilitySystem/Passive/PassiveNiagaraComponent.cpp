// CopyrightHATO


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

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
	if (BoundASC.IsValid() && BoundHandle.IsValid())
	{
		BoundASC->ActivatePassiveAbility.Remove(BoundHandle);
	}
	BoundASC = nullptr;
	BoundHandle.Reset();

	if (UAuraAbilitySystemComponent* Aura = Cast<UAuraAbilitySystemComponent>(InASC))
	{
		BoundHandle = Aura->ActivatePassiveAbility.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		BoundASC = Aura;
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
