// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/CombatInterface.h"
#include "AuraHealthComponent.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTakenSignature, float, DamageAmount);

// OnDeathStarted는 CombatInterface의 FOnDeathSignature를 그대로 사용 — 외부(AuraBeamSpell·DebuffNiagara) 호환

UCLASS(ClassGroup=("Aura"), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAuraHealthComponent();

	/** ASC 초기화 후 호출 — Attribute 변경 감지 바인딩 */
	void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDeadOrDying() const { return bDead; }

	/** 사망 시작 — OnDeathStarted 방송 + DeathTimer 시작 */
	void StartDeath(const FVector& DeathImpulse);

	/** 데미지 전파 (CharacterBase::TakeDamage에서 호출) */
	void BroadcastDamageTaken(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDeathSignature OnDeathStarted;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDamageTakenSignature OnDamageTaken;

	/** 사망 타이머 만료 알림 (C++ 전용, AAuraCharacter::HandlePlayerDeathFinished 등에서 바인딩) */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeathFinished, AActor*);
	FOnDeathFinished OnDeathFinishedNative;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category="Death")
	float DeathTime = 5.f;

	bool bDead = false;
	FTimerHandle DeathTimer;

	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnDeathTimerExpired();
};
