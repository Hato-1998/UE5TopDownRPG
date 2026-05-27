// CopyrightHATO


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Character.h"
#include "Player/AuraClickMovementComponent.h"
#include "Player/AuraCursorTargetingComponent.h"
#include "Player/AuraDamageNumberComponent.h"
#include "Player/AuraMagicCircleComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	CursorTargetingComponent = CreateDefaultSubobject<UAuraCursorTargetingComponent>("CursorTargetingComponent");
	ClickMovementComponent = CreateDefaultSubobject<UAuraClickMovementComponent>("ClickMovementComponent");
	MagicCircleComponent = CreateDefaultSubobject<UAuraMagicCircleComponent>("MagicCircleComponent");
	DamageNumberComponent = CreateDefaultSubobject<UAuraDamageNumberComponent>("DamageNumberComponent");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (!IsLocalController()) return;

	CursorTrace();
	UpdateMagicCircleLocation();
	ClickMovementComponent->TickAutoRun(GetPawn<APawn>());
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	MagicCircleComponent->ShowMagicCircle(AuraMagicCircleClass, DecalMaterial);
}

void AAuraPlayerController::HideMagicCircle()
{
	MagicCircleComponent->HideMagicCircle();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsLocalController())
	{
		DamageNumberComponent->ShowDamageNumber(
			DamageTextComponentClass, DamageAmount, TargetCharacter, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalController()) return;
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(
		MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAction(
		ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(
		ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

	AuraInputComponent->BindAbilityActions(
		InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed,
		&AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (IsInputBlocked(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;

	ClickMovementComponent->StopAutoRun();

	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	FVector ForwardDirection;
	FVector RightDirection;
	GetCameraRelativeMovementDirections(ForwardDirection, RightDirection);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::GetCameraRelativeMovementDirections(FVector& OutForwardDirection, FVector& OutRightDirection) const
{
	const FRotator CameraRotation = PlayerCameraManager ? PlayerCameraManager->GetCameraRotation() : FRotator::ZeroRotator;

	OutForwardDirection = FRotationMatrix(FRotator(0.f, CameraRotation.Yaw, 0.f)).GetUnitAxis(EAxis::X);
	OutRightDirection = FRotationMatrix(FRotator(0.f, CameraRotation.Yaw, 0.f)).GetUnitAxis(EAxis::Y);
}

void AAuraPlayerController::CursorTrace()
{
	if (IsInputBlocked(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		CursorTargetingComponent->ClearHighlightedActors();
		return;
	}

	CursorTargetingComponent->TraceCursor(MagicCircleComponent->IsShowingMagicCircle());
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag Tag)
{
	if (IsInputBlocked(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;

	if (Tag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		AActor* TargetActor = CursorTargetingComponent->GetTargetActor();
		if (IsValid(TargetActor))
		{
			TargetingStatus = TargetActor->Implements<UCombatInterface>()
				? ETargetingStatus::TargetingEnemy
				: ETargetingStatus::TargetingMapEntrance;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
		ClickMovementComponent->StopAutoRun();
	}

	if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
	{
		AuraASC->AbilityInputTagPressed(Tag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag Tag)
{
	if (IsInputBlocked(FAuraGameplayTags::Get().Player_Block_InputReleased)) return;

	if (!Tag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
		{
			AuraASC->AbilityInputTagReleased(Tag);
		}
		return;
	}

	if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
	{
		AuraASC->AbilityInputTagReleased(Tag);
	}

	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		ClickMovementComponent->UpdateCachedDestination(
			CursorTargetingComponent->GetCursorHit(), CursorTargetingComponent);
		UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
		const bool bSpawnClickEffect =
			!CursorTargetingComponent->GetTargetActor() &&
			AuraASC &&
			!AuraASC->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased);
		ClickMovementComponent->TryStartAutoRun(this, GetPawn<APawn>(), bSpawnClickEffect, ClickNiagaraSystem);
		ClickMovementComponent->ResetFollowTime();
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag Tag)
{
	if (IsInputBlocked(FAuraGameplayTags::Get().Player_Block_InputHeld)) return;

	if (!Tag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
		{
			AuraASC->AbilityInputTagHeld(Tag);
		}
		return;
	}

	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
		{
			AuraASC->AbilityInputTagHeld(Tag);
		}
	}
	else
	{
		ClickMovementComponent->AddFollowTime(GetWorld()->GetDeltaSeconds());
		ClickMovementComponent->UpdateCachedDestination(
			CursorTargetingComponent->GetCursorHit(), CursorTargetingComponent);
		ClickMovementComponent->MoveTowardCachedDestination(GetPawn<APawn>());
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

bool AAuraPlayerController::IsInputBlocked(const FGameplayTag& BlockTag)
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
	return AuraASC && AuraASC->HasMatchingGameplayTag(BlockTag);
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	MagicCircleComponent->UpdateMagicCircleLocation(CursorTargetingComponent->GetCursorHit());
}
