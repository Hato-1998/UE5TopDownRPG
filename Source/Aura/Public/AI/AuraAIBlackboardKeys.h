// CopyrightHATO

#pragma once

#include "CoreMinimal.h"

/**
 * AuraAIBlackboardKeys
 *
 * Blackboard key name constants used by AuraAIController / AuraEnemy.
 * Names must match the keys defined in the Blackboard asset (BB_AuraEnemy etc.).
 */
namespace AuraBBKeys
{
	static const FName HitReacting = TEXT("HitReacting");
	static const FName RangedAttacker = TEXT("RangedAttacker");
	static const FName Dead = TEXT("Dead");
}
