// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "AuraDebuffNiagaraComponent.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UAuraDebuffNiagaraComponent();

	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	FGameplayTag DebuffTag;

protected:
	virtual void BeginPlay() override;

	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnOwnerDeath(AActor* DestroyedActor);

};
