// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UPassiveNiagaraComponent;
class UAuraDebuffNiagaraComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UNiagaraSystem;
class USoundBase;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override { return OnAscRegistered; }
	virtual FOnDeathSignature& GetOnDeathDelegate() override { return OnDeathDelegate; }
	virtual FOnDamageSignature& GetOnDamageSignature() override { return OnDamageDelegate; }

	FOnASCRegistered OnAscRegistered;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnDeathSignature OnDeathDelegate;

	FOnDamageSignature OnDamageDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly, Category = "Combat")
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly, Category = "Combat")
	bool bIsBurned = false;

	UPROPERTY(ReplicatedUsing=OnRep_BeingShocked, BlueprintReadOnly, Category = "Combat")
	bool bIsBeingShocked = false;

	virtual void StunTagChanged(const FGameplayTag CallBackTag, int32 NewCount);

	UFUNCTION()
	virtual void OnRep_Stunned();

	virtual void BurnTagChanged(const FGameplayTag CallBackTag, int32 NewCount);

	UFUNCTION()
	virtual void OnRep_Burned();

	UFUNCTION()
	virtual void OnRep_BeingShocked();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();
	virtual void UpdateMovementSpeedFromDebuffs();

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// CombatInterface
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;

	virtual void Die(const FVector& DeathImpulse) override;

	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;

	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;

	virtual int32 GetSummonCount_Implementation() const override;

	virtual void IncrementSummonCount_Implementation(int32 Amount) override;

	virtual bool IsBeingShocked_Implementation() const override;
	virtual void SetBeingShocked_Implementation(bool bBeingShocked) override;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	void AddCharacterAbilities() const;

	/* Dissolve Effect */

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* DeathSound;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bDead = false;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;

	virtual void OnDeathTimerExpired();

	int32 SummonCount = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseWalkSpeed = 600.f;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UAuraDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UAuraDebuffNiagaraComponent> StunDebuffComponent;
private:

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(VisibleAnywhere, Category = "Passive")
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Passive")
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Passive")
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;
};
