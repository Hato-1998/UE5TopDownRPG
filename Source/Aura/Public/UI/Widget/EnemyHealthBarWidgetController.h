// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraWidgetController.h"
#include "AbilitySystem/AuraHealthComponent.h"
#include "EnemyHealthBarWidgetController.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AURA_API UEnemyHealthBarWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	void SetEnemyHealthComponent(UAuraHealthComponent* InHealthComp);

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthAttributeChangedSignature OnEnemyHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthAttributeChangedSignature OnEnemyMaxHealthChanged;

protected:
	UPROPERTY()
	TObjectPtr<UAuraHealthComponent> EnemyHealthComponent;

	UFUNCTION()
	void HandleEnemyHealthChanged(float NewValue);

	UFUNCTION()
	void HandleEnemyMaxHealthChanged(float NewValue);
};
