// CopyrightHATO


#include "AbilitySystem/ExecCalc/ExecCalcDamage.h"

#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

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

	// 데미지 정보 불러오기 — 속성 저항 적용
	float Damage = 0.f;

	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> ResTagToCaptureDef;
	ResTagToCaptureDef.Add(Tags.Attribute_Secondary_ResFire, DamageStatics().ResFireDef);
	ResTagToCaptureDef.Add(Tags.Attribute_Secondary_ResLightning, DamageStatics().ResLightningDef);
	ResTagToCaptureDef.Add(Tags.Attribute_Secondary_ResArcane, DamageStatics().ResArcaneDef);
	ResTagToCaptureDef.Add(Tags.Attribute_Secondary_ResPhysical, DamageStatics().ResPhysicalDef);

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : Tags.DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		checkf(ResistanceTag.IsValid(), TEXT("ResistanceTag is not valid"));

		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);
		if (DamageTypeValue <= 0.f) continue;

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			ResTagToCaptureDef[ResistanceTag], EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		Damage += DamageTypeValue * (100.f - Resistance) / 100.f;
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

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetEffectContext();

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);

	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatarActor);

	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalcCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalcCurveTable->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

	TargetArmor = (TargetArmor * EffectiveArmorCoefficient) - (SourceArmorPenetration * ArmorPenetrationCoefficient);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	Damage -= TargetArmor;

	Damage = FMath::Max(Damage, 1.f);

	const FGameplayModifierEvaluatedData EvalData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvalData);
}
