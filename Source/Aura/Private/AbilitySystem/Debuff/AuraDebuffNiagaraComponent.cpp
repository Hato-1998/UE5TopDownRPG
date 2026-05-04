// CopyrightHATO


#include "AbilitySystem/Debuff/AuraDebuffNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UAuraDebuffNiagaraComponent::UAuraDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UAuraDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraDebuffNiagaraComponent::OnOwnerDeath);
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &UAuraDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
				this, &UAuraDebuffNiagaraComponent::DebuffTagChanged);
		});
	}
}

void UAuraDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UAuraDebuffNiagaraComponent::OnOwnerDeath(AActor* DestroyedActor)
{
	Deactivate();
}
