// CopyrightHATO


#include "Player/AuraPlayerController.h"
#include "Character/AuraCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor= true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhnaInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhnaInputComponent->BindAction(
		MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(
		ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/**
	 * HIT된 Actor에 따른 시나리오
	 * 1. LastActor가 null이고 ThisActor도 null일 경우 (두 액터다 해당 인터페이스를 가지고 있지 않음)
	 * -> 행동 없음
	 * 2. LastActor가 null이고 ThisActor가 valid일 경우 (현재 엑터만 해당 인터페이스를 가지고 있음)
	 * -> ThisActor에 HighLight 실행
	 * 3. LastActor가 valid이고 ThisActor가 null일 경우 (마우스가 지나간 액터에만 해당 인터페이스를 가지고 있음)
	 * -> LastActor에 UnHighLight 실행
	 * 4. LastActor가 Valid이고 ThisActor도 Vaild일 경우 + 두 액터가 다를 경우 (두 액터다 해당 인터페이스를 가지고 있지 않음)
	 * -> ThisActor에 HighLight 실행, LastActor에 UnHighLight 실행
	 * 5. LastActor가 Valid이고 ThisActor도 Vaild일 경우 + 두 액터가 같을 경우 (두 액터다 해당 인터페이스를 가지고 있지 않음)
	 * -> 행동 없음
	 */

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// 2번
			ThisActor->HighLightActor();
		}
	}
	else
	{
		if (ThisActor == nullptr)
		{
			// 3번
			LastActor->UnHighLightActor();
		}
		else
		{
			if (LastActor != ThisActor)
			{
				// 4번
				LastActor->UnHighLightActor();
				ThisActor->HighLightActor();
			}
		}
	}

}
