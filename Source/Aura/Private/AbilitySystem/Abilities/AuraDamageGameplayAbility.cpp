// CopyrightHATO


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	UAuraAbilitySystemLibrary::ApplyGameplayEffect(MakeDamageEffectParamsFromClassDefaults(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;

	AActor* SourceAvatarActor = GetAvatarActorFromActorInfo();
	if (SourceAvatarActor && TargetActor)
	{
		const FVector ToTarget = TargetActor->GetActorLocation() - SourceAvatarActor->GetActorLocation();
		const FVector DirectionToTarget = ToTarget.GetSafeNormal2D();

		if (!DirectionToTarget.IsNearlyZero())
		{
			Params.DeathImpulse = DirectionToTarget * DeathImpulseMagnitude;

			if (const bool bKnockback = FMath::FRandRange(1.f, 100.f) < KnockbackChance)
			{
				FRotator KnockbackRotation = DirectionToTarget.Rotation();
				KnockbackRotation.Pitch = 45.f;
				Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
			}
		}
	}

	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontage(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[RandomIndex];
	}
	return FTaggedMontage();
}
