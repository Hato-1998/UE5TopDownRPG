// CopyrightHATO


#include "Actor/AuraMagicCircle.h"

#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AAuraMagicCircle::AAuraMagicCircle()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(SceneRoot);
}
