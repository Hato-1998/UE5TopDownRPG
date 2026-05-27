// CopyrightHATO


#include "Player/AuraMagicCircleComponent.h"

#include "Actor/AuraMagicCircle.h"
#include "Components/DecalComponent.h"

void UAuraMagicCircleComponent::ShowMagicCircle(
	TSubclassOf<AAuraMagicCircle> MagicCircleClass,
	UMaterialInterface* DecalMaterial)
{
	if (IsValid(MagicCircle) || !MagicCircleClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	MagicCircle = World->SpawnActor<AAuraMagicCircle>(MagicCircleClass);
	if (MagicCircle && DecalMaterial)
	{
		MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
	}
}

void UAuraMagicCircleComponent::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
	MagicCircle = nullptr;
}

void UAuraMagicCircleComponent::UpdateMagicCircleLocation(const FHitResult& CursorHit) const
{
	if (IsValid(MagicCircle) && CursorHit.bBlockingHit)
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

bool UAuraMagicCircleComponent::IsShowingMagicCircle() const
{
	return IsValid(MagicCircle);
}
