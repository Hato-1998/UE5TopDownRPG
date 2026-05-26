// CopyrightHATO


#include "AbilitySystem/ExecCalc/ExecCalcDamage.h"

#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

namespace AuraDamageCurveKeys
{
	static const FName ArmorPenetration(TEXT("ArmorPenetration"));
	static const FName EffectiveArmor(TEXT("EffectiveArmor"));
}

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitReduction);

	DECLARE_ATTRIBUTE_CAPTUREDEF(ResFire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResLightning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResArcane);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResPhysical);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitReduction, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResFire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResLightning, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResArcane, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResPhysical, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}


UExecCalcDamage::UExecCalcDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitReductionDef);

	RelevantAttributesToCapture.Add(DamageStatics().ResFireDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResLightningDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResArcaneDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResPhysicalDef);
}

void UExecCalcDamage::DetermineDebuff(const FGameplayEffectSpec& Spec, const FAuraGameplayTags& Tags) const
{
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : Tags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;

		const float DamageTypeMag = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (DamageTypeMag <= -.5f) continue;

		const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(Tags.Debuff_Chance, false, 0.f);
		const bool bDebuff = FMath::FRandRange(0.f, 100.f) <= SourceDebuffChance;
		if (!bDebuff) continue;

		FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

		UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
		UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

		const float DebuffDamage = Spec.GetSetByCallerMagnitude(Tags.Debuff_Damage, false, 0.f);
		const float DebuffDuration = Spec.GetSetByCallerMagnitude(Tags.Debuff_Duration, false, 0.f);
		const float DebuffFrequency = Spec.GetSetByCallerMagnitude(Tags.Debuff_Frequency, false, 0.f);

		UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
		UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
		UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);

		break;
	}
}

void UExecCalcDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatarActor = SourceASC->GetAvatarActor();
	AActor* TargetAvatarActor = TargetASC->GetAvatarActor();

	const int32 SourcePlayerLevel = (SourceAvatarActor && SourceAvatarActor->Implements<UCombatInterface>())
		? ICombatInterface::Execute_GetPlayerLevel(SourceAvatarActor)
		: 1;
	const int32 TargetPlayerLevel = (TargetAvatarActor && TargetAvatarActor->Implements<UCombatInterface>())
		? ICombatInterface::Execute_GetPlayerLevel(TargetAvatarActor)
		: 1;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Debuff
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

	DetermineDebuff(Spec, Tags);

	// 데미지 정보 불러오기 — 속성 저항 적용
	float Damage = 0.f;

	static const TMap<FGameplayTag, const FGameplayEffectAttributeCaptureDefinition*> ResTagToCaptureDef = []
	{
		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		TMap<FGameplayTag, const FGameplayEffectAttributeCaptureDefinition*> Map;
		Map.Add(AuraTags.Attribute_Secondary_ResFire,      &DamageStatics().ResFireDef);
		Map.Add(AuraTags.Attribute_Secondary_ResLightning, &DamageStatics().ResLightningDef);
		Map.Add(AuraTags.Attribute_Secondary_ResArcane,    &DamageStatics().ResArcaneDef);
		Map.Add(AuraTags.Attribute_Secondary_ResPhysical,  &DamageStatics().ResPhysicalDef);
		return Map;
	}();

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : Tags.DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		checkf(ResistanceTag.IsValid(), TEXT("ResistanceTag is not valid"));

		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);
		if (DamageTypeValue <= 0.f) continue;

		const FGameplayEffectAttributeCaptureDefinition* CaptureDef = ResTagToCaptureDef.FindRef(ResistanceTag);
		if (!CaptureDef) continue;

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(*CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		Damage += DamageTypeValue * (100.f - Resistance) / 100.f;
	}

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetEffectContext();

	// 저항 적용 후 데미지가 0 이하이면 이후 falloff/crit/block/armor 계산 전부 생략 (최소 1 데미지 정책 유지)
	if (Damage <= 0.f)
	{
		Damage = 1.f;
		const FGameplayModifierEvaluatedData EvalData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
		OutExecutionOutput.AddOutputModifier(EvalData);
		return;
	}

	// Radial Damage falloff
	if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
	{
		if (ICombatInterface* CombatTarget = Cast<ICombatInterface>(TargetAvatarActor))
		{
			FDelegateHandle DamageHandle = CombatTarget->GetOnDamageSignature().AddLambda(
				[&Damage](float DamageAmount) { Damage = DamageAmount; });

			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatarActor,
				Damage,
				0.f,
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatarActor,
				nullptr);

			CombatTarget->GetOnDamageSignature().Remove(DamageHandle);
		}
	}

	// 크리 확률 가져오기, 크리시 데미지 2배 + 크리데미지로 보정
	float CriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, CriticalHitChance);
	CriticalHitChance = FMath::Clamp(CriticalHitChance, 0.f, 100.f);

	float CriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, CriticalHitDamage);
	CriticalHitDamage = FMath::Max(CriticalHitDamage, 0.f);

	float CriticalHitReduction = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitReductionDef, EvaluateParameters, CriticalHitReduction);
	CriticalHitReduction = FMath::Max(CriticalHitReduction, 0.f);

	const bool bCriticalHit = FMath::FRandRange(0.f, 100.f) <= (CriticalHitChance - CriticalHitReduction);
	if (bCriticalHit)
	{
		Damage *= 2.f + (CriticalHitDamage * 0.1f);
	}

	// 가드 확률 가져오기, 가드 확률에 따라 성공시 피해량 절반
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Clamp(TargetBlockChance, 0.f, 99.f);

	const bool bBlocked = FMath::FRandRange(0.f, 100.f) <= TargetBlockChance;
	if (bBlocked)
	{
		Damage *= 0.5f;
	}

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);

	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatarActor);

	float ArmorPenetrationCoefficient = 1.f;
	float EffectiveArmorCoefficient = 1.f;

	if (!CharacterClassInfo)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: CharacterClassInfo is not set. Using default armor coefficients."), __FUNCTION__);
	}
	else if (!CharacterClassInfo->DamageCalcCurveTable)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: DamageCalcCurveTable is not set on %s. Using default armor coefficients."),
			__FUNCTION__, *GetNameSafe(CharacterClassInfo));
	}
	else
	{
		if (const FRealCurve* ArmorPenetrationCurve =
			CharacterClassInfo->DamageCalcCurveTable->FindCurve(AuraDamageCurveKeys::ArmorPenetration, FString()))
		{
			ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
		}
		else
		{
			UE_LOG(LogAura, Warning, TEXT("%hs: ArmorPenetration curve row is missing from %s. Using default coefficient."),
				__FUNCTION__, *GetNameSafe(CharacterClassInfo->DamageCalcCurveTable));
		}

		if (const FRealCurve* EffectiveArmorCurve =
			CharacterClassInfo->DamageCalcCurveTable->FindCurve(AuraDamageCurveKeys::EffectiveArmor, FString()))
		{
			EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
		}
		else
		{
			UE_LOG(LogAura, Warning, TEXT("%hs: EffectiveArmor curve row is missing from %s. Using default coefficient."),
				__FUNCTION__, *GetNameSafe(CharacterClassInfo->DamageCalcCurveTable));
		}
	}

	TargetArmor = (TargetArmor * EffectiveArmorCoefficient) - (SourceArmorPenetration * ArmorPenetrationCoefficient);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	Damage -= TargetArmor;

	Damage = FMath::Max(Damage, 1.f);

	const FGameplayModifierEvaluatedData EvalData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvalData);
}
