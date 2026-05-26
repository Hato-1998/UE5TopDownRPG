// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

class AAuraEnemy;

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Spawn Point")
	void SpawnEnemy();

	UPROPERTY(EditAnywhere, Category = "Spawn Point")
	TSubclassOf<AAuraEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Spawn Point")
	int32 EnemyLevel = 1;

	UPROPERTY(EditAnywhere, Category = "Spawn Point")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
};
