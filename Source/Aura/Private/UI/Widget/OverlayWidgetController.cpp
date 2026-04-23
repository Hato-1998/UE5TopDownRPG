// CopyrightHATO


#include "UI/Widget/OverlayWidgetController.h"

#include "AttributeSet.h"
#include "Engine/DataTable.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/AuraLevelUpInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(PlayerState))
	{
		const int32 CurrentLevel = AuraPlayerState->GetPlayerLevel();
		const int32 CurrentXP = AuraPlayerState->GetXP();
		OnPlayerLevelChangedDelegate.Broadcast(CurrentLevel);
		if (const ULevelUpInfo* LevelUpInfo = AuraPlayerState->GetLevelUpInfo())
		{
			OnXPPercentChangedDelegate.Broadcast(LevelUpInfo->GetXPBarPercent(CurrentLevel, CurrentXP));
		}
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});

	if (UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& AssetTag : AssetTags)
				{
					//expanding out parent tags "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
					//해당 Tag가 포함되어 있는가를 확인할 때, 상위 계층의 태그를 검색하면 True를 반환하지만, 하위 계층의 태그는 false 반환
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (AssetTag.MatchesTag(MessageTag))
					{
						FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, AssetTag);
						checkf(Row, TEXT("Row not found for tag [%s]"), *AssetTag.ToString());
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			});
	}

	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(PlayerState))
	{
		AuraPlayerState->OnXPChangedDelegate.AddLambda(
			[this, AuraPlayerState](int32 NewXP)
			{
				if (const ULevelUpInfo* LevelUpInfo = AuraPlayerState->GetLevelUpInfo())
				{
					const int32 CurrentLevel = AuraPlayerState->GetPlayerLevel();
					OnXPPercentChangedDelegate.Broadcast(LevelUpInfo->GetXPBarPercent(CurrentLevel, NewXP));
				}
			});

		AuraPlayerState->OnLevelChangedDelegate.AddLambda(
			[this, AuraPlayerState](int32 NewLevel)
			{
				OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
				// 레벨업 시 XP Bar도 재계산 (새 구간 시작)
				if (const ULevelUpInfo* LevelUpInfo = AuraPlayerState->GetLevelUpInfo())
				{
					OnXPPercentChangedDelegate.Broadcast(LevelUpInfo->GetXPBarPercent(NewLevel, AuraPlayerState->GetXP()));
				}
			});
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC) const
{
	if (!AuraASC->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, AuraASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraASC->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoInitializedDelegate.Broadcast(Info);
	});
	AuraASC->ForEachAbility(BroadcastDelegate);
}
