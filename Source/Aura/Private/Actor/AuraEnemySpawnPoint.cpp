// CopyrightHATO


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	checkf(EnemyClass, TEXT("%hs: EnemyClass is not set on %s."), __FUNCTION__, *GetNameSafe(this));

	const FTransform SpawnTransform = GetActorTransform();
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(
		EnemyClass,
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->SpawnDefaultController();
	Enemy->FinishSpawning(SpawnTransform);
}
