// CopyrightHATO


#include "Player/AuraCursorTargetingComponent.h"

#include "Aura/Aura.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/HighLightInterface.h"

void UAuraCursorTargetingComponent::TraceCursor(bool bExcludePlayers)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (!PlayerController) return;

	const ECollisionChannel TraceChannel = bExcludePlayers ? ECC_ExcludePlayers : ECC_Visibility;
	PlayerController->GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = CurrentActor;
	AActor* HitActor = CursorHit.GetActor();
	CurrentActor = IsValid(HitActor) && HitActor->Implements<UHighLightInterface>() ? HitActor : nullptr;

	if (LastActor != CurrentActor)
	{
		UnHighlightActor(LastActor);
		HighlightActor(CurrentActor);
	}
}

void UAuraCursorTargetingComponent::ClearHighlightedActors()
{
	UnHighlightActor(LastActor);
	UnHighlightActor(CurrentActor);
	LastActor = nullptr;
	CurrentActor = nullptr;
}

void UAuraCursorTargetingComponent::OverrideMoveToLocationOnTarget(FVector& Destination) const
{
	if (IsValid(CurrentActor) && CurrentActor->Implements<UHighLightInterface>())
	{
		IHighLightInterface::Execute_SetMoveToLocation(CurrentActor, Destination);
	}
}

void UAuraCursorTargetingComponent::HighlightActor(AActor* InActor) const
{
	if (IsValid(InActor) && InActor->Implements<UHighLightInterface>())
	{
		IHighLightInterface::Execute_HighLightActor(InActor);
	}
}

void UAuraCursorTargetingComponent::UnHighlightActor(AActor* InActor) const
{
	if (IsValid(InActor) && InActor->Implements<UHighLightInterface>())
	{
		IHighLightInterface::Execute_UnHighLightActor(InActor);
	}
}
