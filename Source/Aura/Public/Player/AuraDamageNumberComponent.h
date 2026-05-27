// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraDamageNumberComponent.generated.h"

class UDamageTextComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraDamageNumberComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void ShowDamageNumber(
		TSubclassOf<UDamageTextComponent> DamageTextComponentClass,
		float DamageAmount,
		ACharacter* TargetCharacter,
		bool bBlockedHit,
		bool bCriticalHit) const;
};
