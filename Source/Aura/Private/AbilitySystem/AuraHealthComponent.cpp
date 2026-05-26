// CopyrightHATO

#include "AbilitySystem/AuraHealthComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "TimerManager.h"

UAuraHealthComponent::UAuraHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UAuraHealthComponent::InitializeWithAbilitySystem(UAbilitySystemComponent* InASC)
{
	if (!InASC)
	{
		return;
	}
	AbilitySystemComponent = InASC;

	const UAuraAttributeSet* AuraAS = AbilitySystemComponent->GetSet<UAuraAttributeSet>();
	if (!AuraAS)
	{
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
		.AddUObject(this, &UAuraHealthComponent::HandleHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute())
		.AddUObject(this, &UAuraHealthComponent::HandleMaxHealthChanged);

	// 초기값 브로드캐스트
	OnHealthChanged.Broadcast(AuraAS->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
}

float UAuraHealthComponent::GetHealth() const
{
	if (!AbilitySystemComponent) return 0.f;
	const UAuraAttributeSet* AuraAS = AbilitySystemComponent->GetSet<UAuraAttributeSet>();
	return AuraAS ? AuraAS->GetHealth() : 0.f;
}

float UAuraHealthComponent::GetMaxHealth() const
{
	if (!AbilitySystemComponent) return 0.f;
	const UAuraAttributeSet* AuraAS = AbilitySystemComponent->GetSet<UAuraAttributeSet>();
	return AuraAS ? AuraAS->GetMaxHealth() : 0.f;
}

void UAuraHealthComponent::StartDeath(const FVector& DeathImpulse)
{
	if (bDead)
	{
		return;
	}
	bDead = true;

	AActor* Owner = GetOwner();
	OnDeathStarted.Broadcast(Owner);

	if (Owner && Owner->HasAuthority())
	{
		Owner->GetWorldTimerManager().SetTimer(DeathTimer, this, &UAuraHealthComponent::OnDeathTimerExpired, DeathTime, false);
	}
}

void UAuraHealthComponent::BroadcastDamageTaken(float DamageAmount)
{
	OnDamageTaken.Broadcast(DamageAmount);
}

void UAuraHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UAuraHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UAuraHealthComponent::OnDeathTimerExpired()
{
	OnDeathFinishedNative.Broadcast(GetOwner());
}
