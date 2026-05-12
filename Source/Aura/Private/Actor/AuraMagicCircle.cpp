// CopyrightHATO


#include "Actor/AuraMagicCircle.h"

#include "Components/DecalComponent.h"

// Sets default values
AAuraMagicCircle::AAuraMagicCircle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AAuraMagicCircle::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAuraMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
