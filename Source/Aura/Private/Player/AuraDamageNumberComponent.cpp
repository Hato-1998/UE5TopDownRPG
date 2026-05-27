// CopyrightHATO


#include "Player/AuraDamageNumberComponent.h"

#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

void UAuraDamageNumberComponent::ShowDamageNumber(
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass,
	float DamageAmount,
	ACharacter* TargetCharacter,
	bool bBlockedHit,
	bool bCriticalHit) const
{
	if (!IsValid(TargetCharacter) || !DamageTextComponentClass) return;

	UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
	if (!DamageText) return;

	DamageText->RegisterComponent();
	DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
}
