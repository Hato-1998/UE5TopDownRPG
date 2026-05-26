// CopyrightHATO


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

	// Primary Attributes
	TagsToAttributes.Add(Tags.Attribute_Primary_Strength, GetStrengthAttribute());
	TagsToAttributes.Add(Tags.Attribute_Primary_Intelligence, GetIntelligenceAttribute());
	TagsToAttributes.Add(Tags.Attribute_Primary_Resilience, GetResilienceAttribute());
	TagsToAttributes.Add(Tags.Attribute_Primary_Vigor, GetVigorAttribute());

	// Secondary Attributes
	TagsToAttributes.Add(Tags.Attribute_Secondary_Armor, GetArmorAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_CriticalHitReduction, GetCriticalHitReductionAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_MaxMana, GetMaxManaAttribute());

	// Resistance Attributes
	TagsToAttributes.Add(Tags.Attribute_Secondary_ResFire, GetResFireAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_ResLightning, GetResLightningAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_ResArcane, GetResArcaneAttribute());
	TagsToAttributes.Add(Tags.Attribute_Secondary_ResPhysical, GetResPhysicalAttribute());

	// Vital Attributes
	TagsToAttributes.Add(Tags.Attribute_Vital_Health, GetHealthAttribute());
	TagsToAttributes.Add(Tags.Attribute_Vital_Mana, GetManaAttribute());
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams SharedParams;
    SharedParams.bIsPushBased = true;
    SharedParams.Condition = COND_None;
    SharedParams.RepNotifyCondition = REPNOTIFY_Always;

	/*
	* 기본 속성
	*/
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Resilience, SharedParams);
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Strength, SharedParams);
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Intelligence, SharedParams);
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Vigor, SharedParams);

	/*
	 * 서브 속성
	*/
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Armor, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, ArmorPenetration, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, BlockChance, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, CriticalHitChance, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, CriticalHitDamage, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, CriticalHitReduction, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, HealthRegeneration, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, ManaRegeneration, SharedParams);

	/*
	 * 저항 속성
	*/
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, ResFire, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, ResLightning, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, ResArcane, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, ResPhysical, SharedParams);

	/*
	* 상태 속성
	*/
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Health, SharedParams);
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, MaxHealth, SharedParams);

    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, Mana, SharedParams);
    DOREPLIFETIME_WITH_PARAMS_FAST(UAuraAttributeSet, MaxMana, SharedParams);
}

//속성 변경이 시작되기 전에 먼저 호출
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = 시전, 원인 제공자, target = 대상자 (AS를 가진 owner)
	Props.EffectContext = Data.EffectSpec.GetContext();

	Props.SourceASC = Props.EffectContext.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid() )
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = FMath::Clamp(GetHealth() - LocalIncomingDamage, 0.f, GetMaxHealth());
		SetHealth(NewHealth);

		const bool bFatal = NewHealth <= 0.f;

		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContext);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContext);
		ShowFloatingText(Props, LocalIncomingDamage, bBlockedHit, bCriticalHit);

		if (bFatal)
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
			{
				const FVector DeathImpulse = UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContext);
				CombatInterface->Die(DeathImpulse);
			}
			SendXPEvent(Props);
		}
		else
		{
			const bool bIsDebuffDamage = UAuraAbilitySystemLibrary::IsDebuffDamage(Props.EffectContext);

			if (Props.TargetASC &&
				Props.TargetCharacter &&
				!bIsDebuffDamage &&
				Props.TargetCharacter->Implements<UCombatInterface>() &&
				!ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter))
			{
				FGameplayTagContainer Tags;
				Tags.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(Tags));
			}

			const FVector KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContext);
			if (!KnockbackForce.IsNearlyZero(1.f) && Props.TargetCharacter)
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}

			if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContext))
			{
				Debuff(Props);
			}
		}
	}
}

void UAuraAttributeSet::HandleIncomingXp(const FEffectProperties& Props)
{
	const int32 LocalIncomingXP = GetIncomingXp();
	SetIncomingXp(0.f);

	if (Props.SourceCharacter && Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumLevelUp = NewLevel - CurrentLevel;

		if (NumLevelUp > 0)
		{
			IPlayerInterface::Execute_AddToLevel(Props.SourceCharacter, NumLevelUp);

			int32 AttributePointsReward = 0;
			int32 SpellPointsReward = 0;
			for (int32 LevelUpCount = 0; LevelUpCount < NumLevelUp; LevelUpCount++)
			{
				AttributePointsReward += IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel + LevelUpCount);
				SpellPointsReward += IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel + LevelUpCount);
			}

			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);

			bTopOffHealth = true;
			bTopOffMana = true;
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	if (!Props.SourceASC || !Props.TargetASC)
	{
		return;
	}

	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);
	UAuraAbilitySystemLibrary::SetIsDebuffDamage(EffectContext, true);
	FGameplayEffectContext* RawContext = EffectContext.Get();
	if (!RawContext || RawContext->GetScriptStruct() != FAuraGameplayEffectContext::StaticStruct())
	{
		return;
	}
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(RawContext);

	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContext);
	if (!DamageType.IsValid())
	{
		return;
	}

	const FGameplayTag* DebuffTag = Tags.DamageTypesToDebuffs.Find(DamageType);
	if (!DebuffTag || !DebuffTag->IsValid())
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Missing debuff tag mapping for damage type %s."),
			__FUNCTION__, *DamageType.ToString());
		return;
	}

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(Props.SourceAvatarActor);
	if (!CharacterClassInfo)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: CharacterClassInfo not available for debuff."), __FUNCTION__);
		return;
	}

	const TSubclassOf<UGameplayEffect>* DebuffGEClass = CharacterClassInfo->DebuffEffectClasses.Find(*DebuffTag);
	if (!DebuffGEClass || !*DebuffGEClass)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: No DebuffEffectClass mapped for tag %s. Add it to CharacterClassInfo."),
			__FUNCTION__, *DebuffTag->ToString());
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = Props.SourceASC->MakeOutgoingSpec(*DebuffGEClass, 1.f, EffectContext);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContext);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContext);
	// Period(Frequency)는 UGameplayEffect::Period가 FScalableFloat 타입이라 SetByCaller 미지원.
	// 각 GE_Debuff_* BP의 Period에 고정값(1.0 권장) 설정. 동적 조정이 필요해지면 Custom Calc Class로.

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Damage, DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Duration, DebuffDuration);

	AuraContext->SetDamageType(MakeShared<FGameplayTag>(DamageType));

	Props.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (!Props.TargetAvatarActor || !Props.TargetASC)
	{
		UE_LOG(LogAura, Warning,
			TEXT("%hs: Missing target effect properties. TargetAvatarActor=%s TargetASC=%s Attribute=%s"),
			__FUNCTION__,
			*GetNameSafe(Props.TargetAvatarActor),
			*GetNameSafe(Props.TargetASC),
			*Data.EvaluatedData.Attribute.GetName());
		return;
	}

	if (Props.TargetAvatarActor->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetAvatarActor)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}

	if (Data.EvaluatedData.Attribute == GetIncomingXpAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.f)
		{
			HandleIncomingXp(Props);
		}
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if ( Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if ( Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (!Props.SourceCharacter || !Props.TargetCharacter)
	{
		return;
	}

	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props) const
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);

	if (CombatInterface && Props.SourceCharacter)
	{
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayEventData Payload;

		Payload.EventTag = GameplayTags.Attribute_Meta_IncomingXP;
		Payload.EventMagnitude = CombatInterface->GetXPReward_Implementation();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attribute_Meta_IncomingXP, Payload);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// OnRep 단축 매크로 — 19개 속성의 RepNotify 구현 보일러플레이트 제거
// ─────────────────────────────────────────────────────────────────────────────
#define DEFINE_AURA_ONREP(PropertyName) \
void UAuraAttributeSet::OnRep_##PropertyName(const FGameplayAttributeData& Old##PropertyName) const \
{ \
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PropertyName, Old##PropertyName); \
}

DEFINE_AURA_ONREP(Health)
DEFINE_AURA_ONREP(MaxHealth)
DEFINE_AURA_ONREP(Mana)
DEFINE_AURA_ONREP(MaxMana)
DEFINE_AURA_ONREP(Strength)
DEFINE_AURA_ONREP(Intelligence)
DEFINE_AURA_ONREP(Resilience)
DEFINE_AURA_ONREP(Vigor)
DEFINE_AURA_ONREP(Armor)
DEFINE_AURA_ONREP(ArmorPenetration)
DEFINE_AURA_ONREP(BlockChance)
DEFINE_AURA_ONREP(CriticalHitChance)
DEFINE_AURA_ONREP(CriticalHitDamage)
DEFINE_AURA_ONREP(CriticalHitReduction)
DEFINE_AURA_ONREP(HealthRegeneration)
DEFINE_AURA_ONREP(ManaRegeneration)
DEFINE_AURA_ONREP(ResFire)
DEFINE_AURA_ONREP(ResLightning)
DEFINE_AURA_ONREP(ResArcane)
DEFINE_AURA_ONREP(ResPhysical)

#undef DEFINE_AURA_ONREP
