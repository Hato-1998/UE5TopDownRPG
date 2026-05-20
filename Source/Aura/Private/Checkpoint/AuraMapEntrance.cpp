// CopyrightHATO


#include "Checkpoint/AuraMapEntrance.h"

#include "Aura/AuraLogChannels.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraMapEntrance::AAuraMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AAuraMapEntrance::HighLightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraMapEntrance::LoadActor_Implementation()
{
}

void AAuraMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (!OtherActor->Implements<UPlayerInterface>()) return;

	if (DestinationMap.IsNull())
	{
		UE_LOG(LogAura, Error, TEXT("AAuraMapEntrance %s: DestinationMap is not set"), *GetName())
		return;
	}

	bReached = true;

	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
	}

	IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
}
