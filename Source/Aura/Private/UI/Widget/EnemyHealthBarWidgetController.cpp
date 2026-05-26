// CopyrightHATO

#include "UI/Widget/EnemyHealthBarWidgetController.h"

void UEnemyHealthBarWidgetController::SetEnemyHealthComponent(UAuraHealthComponent* InHealthComp)
{
	EnemyHealthComponent = InHealthComp;
}

void UEnemyHealthBarWidgetController::BroadcastInitialValues()
{
	if (!EnemyHealthComponent) return;
	OnEnemyHealthChanged.Broadcast(EnemyHealthComponent->GetHealth());
	OnEnemyMaxHealthChanged.Broadcast(EnemyHealthComponent->GetMaxHealth());
}

void UEnemyHealthBarWidgetController::BindCallbacksToDependencies()
{
	if (!EnemyHealthComponent) return;
	EnemyHealthComponent->OnHealthChanged.AddDynamic(this, &UEnemyHealthBarWidgetController::HandleEnemyHealthChanged);
	EnemyHealthComponent->OnMaxHealthChanged.AddDynamic(this, &UEnemyHealthBarWidgetController::HandleEnemyMaxHealthChanged);
}

void UEnemyHealthBarWidgetController::HandleEnemyHealthChanged(float NewValue)
{
	OnEnemyHealthChanged.Broadcast(NewValue);
}

void UEnemyHealthBarWidgetController::HandleEnemyMaxHealthChanged(float NewValue)
{
	OnEnemyMaxHealthChanged.Broadcast(NewValue);
}
