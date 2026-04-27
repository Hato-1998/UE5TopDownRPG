// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class ULevelUpInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/);

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	FORCEINLINE	int32 GetPlayerLevel() const { return Level; }

	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE ULevelUpInfo* GetLevelUpInfo() const { return LevelUpInfo; }

	FORCEINLINE int32 GetAttributePoints() const { return AP; }
	FORCEINLINE int32 GetSpellPoints() const { return SP; }

	void SetXP(int32 InXP);
	void AddToXP(int32 InXP);

	void SetLevel(int32 InLevel);
	void AddToLevel(int32 InLevel);

	void SetAttributePoints(int32 InAP);
	void AddToAttributePoints(int32 InAP);

	void SetSpellPoints(int32 InSP);
	void AddToSpellPoints(int32 InSP);

	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;

protected:
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level, Category = "PlayerState")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP, Category = "PlayerState")
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AP, Category = "PlayerState")
	int32 AP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SP, Category = "PlayerState")
	int32 SP = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;

	UFUNCTION()
	void OnRep_AP(int32 OldAP) const;

	UFUNCTION()
	void OnRep_SP(int32 OldSP) const;

};
