// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/HighLightInterface.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyAttributeChangedSignature, float, NewValue);

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighLightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;

	virtual int32 GetPlayerLevel() const override;

	UPROPERTY(BlueprintAssignable)
	FOnEnemyAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnEnemyAttributeChangedSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
