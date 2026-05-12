// CopyrightHATO


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraVolumeRendererProperties.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	UAuraAbilitySystemLibrary::ApplyGameplayEffect(MakeDamageEffectParamsFromClassDefaults(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector InOverrideKnockbackDirection,
	bool bOverrideDeathImpulse, FVector InOverrideDeathImpulse, bool bOverridePitch, float PitchOverride) const
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

	const bool bKnockback = KnockbackChance > 0.f && FMath::FRandRange(1.f, 100.f) < KnockbackChance;

	const AActor* SourceAvatarActor = GetAvatarActorFromActorInfo();
	if (SourceAvatarActor && TargetActor && (!bOverrideKnockbackDirection || !bOverrideDeathImpulse))
	{
		const FVector DirectionToTarget = (TargetActor->GetActorLocation() - SourceAvatarActor->GetActorLocation()).GetSafeNormal2D();

		if (!bOverrideKnockbackDirection && bKnockback)
		{
			Params.KnockbackForce = DirectionToTarget * KnockbackForceMagnitude;
		}
		if (!bOverrideDeathImpulse)
		{
			Params.DeathImpulse = DirectionToTarget * DeathImpulseMagnitude;
		}
	}

	if (bOverrideKnockbackDirection && bKnockback)
	{
		FVector KnockbackDirection = InOverrideKnockbackDirection.GetSafeNormal();
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = KnockbackDirection.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			KnockbackDirection = KnockbackRotation.Vector();
		}
		Params.KnockbackForce = KnockbackDirection * KnockbackForceMagnitude;
	}

	if (bOverrideDeathImpulse)
	{
		FVector DeathImpulseDirection = InOverrideDeathImpulse.GetSafeNormal();
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirection.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			DeathImpulseDirection = DeathImpulseRotation.Vector();
		}
		Params.DeathImpulse = DeathImpulseDirection * DeathImpulseMagnitude;
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = true;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
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
