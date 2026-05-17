// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;
class UAbilitySystemComponent;

UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()


	UPassiveNiagaraComponent();

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

	void BindToASC(UAbilitySystemComponent* InASC);

protected:
	virtual void BeginPlay() override;

	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);

private:
	TWeakObjectPtr<UAuraAbilitySystemComponent> BoundASC;
	FDelegateHandle BoundHandle;
};
