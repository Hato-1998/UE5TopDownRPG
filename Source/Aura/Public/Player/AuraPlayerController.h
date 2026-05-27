// CopyrightHATO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class AAuraMagicCircle;
class UAuraClickMovementComponent;
class UAuraCursorTargetingComponent;
class UAuraDamageNumberComponent;
class UAuraMagicCircleComponent;
class UNiagaraSystem;
class UDamageTextComponent;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ETargetingStatus : uint8
{
	TargetingEnemy,
	TargetingMapEntrance,
	NotTargeting
};

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftKeyDown = true;};
	void ShiftReleased() { bShiftKeyDown = false;};
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);
	void GetCameraRelativeMovementDirections(FVector& OutForwardDirection, FVector& OutRightDirection) const;

	void CursorTrace();

	void AbilityInputTagPressed(const FGameplayTag Tag);
	void AbilityInputTagReleased(const FGameplayTag Tag);
	void AbilityInputTagHeld(const FGameplayTag Tag);

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetAuraASC();
	bool IsInputBlocked(const FGameplayTag& BlockTag);

	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraCursorTargetingComponent> CursorTargetingComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraClickMovementComponent> ClickMovementComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraMagicCircleComponent> MagicCircleComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraDamageNumberComponent> DamageNumberComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraMagicCircle> AuraMagicCircleClass;

	void UpdateMagicCircleLocation();
};
