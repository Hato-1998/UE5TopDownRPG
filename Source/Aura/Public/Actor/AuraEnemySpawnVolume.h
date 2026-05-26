// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class UBoxComponent;
class AAuraEnemySpawnPoint;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	AAuraEnemySpawnVolume();

	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AAuraEnemySpawnPoint>> SpawnPoints;

	/** 한 번 트리거된 볼륨은 재로드 시 재소환되지 않도록 SaveGame 직렬화 */
	UPROPERTY(SaveGame)
	bool bReached = false;

	/**
	 * true면 레벨 재로드 시마다 bReached를 false로 리셋하여 적이 다시 스폰된다 (반복 던전·리스폰 지역용).
	 * false(기본)면 한 번 트리거된 후 영구 비활성 — 기존 동작.
	 * 디자이너가 BP 인스턴스마다 조정 가능.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Spawn",
		meta=(DisplayName="Reset On Load",
		      ToolTip="If true, this volume rearms on level reload (enemies respawn on revisit). If false, once triggered it stays permanently inactive."))
	bool bResetOnLoad = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
