// CopyrightHATO


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Game/AuraSaveGameSubsystem.h"
#include "Interaction/PlayerInterface.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bResetOnLoad)
	{
		// 리셋 모드: SaveGame에 직렬화된 bReached를 무시하고 매 로드마다 재무장
		bReached = false;
		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		return;
	}

	if (bReached)
	{
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AAuraEnemySpawnVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	EnsureSaveId();
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxOverlap);
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (bReached) return;
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	bReached = true;

	for (AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UAuraSaveGameSubsystem* SaveSubsystem = GameInstance->GetSubsystem<UAuraSaveGameSubsystem>())
		{
			SaveSubsystem->SaveWorldState(GetWorld());
		}
	}

	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraEnemySpawnVolume::EnsureSaveId()
{
	if (SaveId.IsValid())
	{
		return;
	}

	SaveId = FGuid::NewGuid();
#if WITH_EDITOR
	Modify();
	MarkPackageDirty();
#endif
}
