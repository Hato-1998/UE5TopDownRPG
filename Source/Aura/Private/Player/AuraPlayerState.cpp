// CopyrightHATO


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	SetNetUpdateFrequency(100.f);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AP);
	DOREPLIFETIME(AAuraPlayerState, SP);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{

}

void AAuraPlayerState::SetXP(int32 InXP)
{
	if (!HasAuthority()) return;
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	if (!HasAuthority()) return;
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	if (!HasAuthority()) return;
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	if (!HasAuthority()) return;
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetAttributePoints(int32 InAP)
{
	if (!HasAuthority()) return;
	AP = InAP;
	OnAttributePointsChangedDelegate.Broadcast(AP);
}

void AAuraPlayerState::AddToAttributePoints(int32 InAP)
{
	if (!HasAuthority()) return;
	AP += InAP;
	OnAttributePointsChangedDelegate.Broadcast(AP);
}

void AAuraPlayerState::SetSpellPoints(int32 InSP)
{
	if (!HasAuthority()) return;
	SP = InSP;
	OnSpellPointsChangedDelegate.Broadcast(SP);
}

void AAuraPlayerState::AddToSpellPoints(int32 InSP)
{
	if (!HasAuthority()) return;
	SP += InSP;
	OnSpellPointsChangedDelegate.Broadcast(SP);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AP(int32 OldAP) const
{
	OnAttributePointsChangedDelegate.Broadcast(AP);
}

void AAuraPlayerState::OnRep_SP(int32 OldSP) const
{
	OnSpellPointsChangedDelegate.Broadcast(SP);
}
