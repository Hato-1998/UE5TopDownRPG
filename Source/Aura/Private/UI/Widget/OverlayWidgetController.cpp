// CopyrightHATO


#include "UI/Widget/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AttributeSet.h"
#include "Engine/DataTable.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/AuraLevelUpInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());

	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());


	const int32 CurrentLevel = GetAuraPS()->GetPlayerLevel();
	const int32 CurrentXP = GetAuraPS()->GetXP();
	OnPlayerLevelChangedDelegate.Broadcast(CurrentLevel);

	if (const ULevelUpInfo* LevelUpInfo = GetAuraPS()->GetLevelUpInfo())
	{
		OnXPPercentChangedDelegate.Broadcast(LevelUpInfo->GetXPBarPercent(CurrentLevel, CurrentXP));
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddLambda(
		[this](int32 NewXP)
		{
			if (const ULevelUpInfo* LevelUpInfo = GetAuraPS()->GetLevelUpInfo())
			{
				const int32 CurrentLevel = GetAuraPS()->GetPlayerLevel();
				OnXPPercentChangedDelegate.Broadcast(LevelUpInfo->GetXPBarPercent(CurrentLevel, NewXP));
			}
		});

	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);

			// 레벨업 시 XP Bar도 재계산 (새 구간 시작)
			if (const ULevelUpInfo* LevelUpInfo = GetAuraPS()->GetLevelUpInfo())
			{
				OnXPPercentChangedDelegate.Broadcast(
					LevelUpInfo->GetXPBarPercent(NewLevel, GetAuraPS()->GetXP()));
			}
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});

	if (GetAuraASC())
	{
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(
				this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& AssetTag : AssetTags)
				{
					//expanding out parent tags "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
					//해당 Tag가 포함되어 있는가를 확인할 때, 상위 계층의 태그를 검색하면 True를 반환하지만, 하위 계층의 태그는 false 반환
					if (AssetTag.MatchesTag(FAuraGameplayTags::Get().Message))
					{
						FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, AssetTag);
						checkf(Row, TEXT("Row not found for tag [%s]"), *AssetTag.ToString());
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			});
	}
}
