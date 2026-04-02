// CopyrightHATO


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"


// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

}

 void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect>& GameplayEffectClass)
{
	//인터페이스를 통해 컴포넌트를 연결시키는 방식도 있음, 라이브러리를 통할지, 액터마다 인터페이스를 만들지는 프로젝트마다 다름
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass)
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContext);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
}
