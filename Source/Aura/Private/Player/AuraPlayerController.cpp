//  Copyright Dagaz84521


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

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
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
		Subsystem->AddMappingContext(AuraContext, 0);
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move); // 注意Triggered是支持按住，Started支持的是按下。所以对于单次点击，则是Started，对于移动这种持续操作，采用Triggered
}

void AAuraPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

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
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;
	Cast<IEnemyInterface>(CursorHit.GetActor());

	// 由于当鼠标点击不同的Actor的时候，需要将上一个Actor的高光取消，同时将现在的高光开启。
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/**
	 *	根据前后两个Actor是否相同，存在以下几种情况
	 *	1. LastActor & ThisActor == null, 即鼠标没有点击任何Actor
	 *		- 不需要做任何处理
	 *	2. LastActor == null, ThisActor != null, 即鼠标点击了一个新的Actor
	 *		- ThisActor Highlight
	 *	3. LastActor != null, ThisActor == null, 即鼠标从一个Actor上移开
	 *		- LastActor UnHighlight
	 *	4. LastActor != null, ThisActor != null, 即鼠标在两个Actor之间移动
	 *		- LastActor UnHighlight, ThisActor Highlight
	 *	5. LastActor == ThisActor, 即鼠标在同一个Actor上移动
	 *		- 不需要做任何处理
	 */
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case 2
			ThisActor->HighlightActor();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Highlight Actor"));
		}
		else // ThisActor == nullptr
		{
			// Case 1
		}
	}
	else // LastActor != nullptr
	{
		if (ThisActor == nullptr)
		{
			// case 3
			LastActor->UnHighlightActor();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UnHighlight Actor"));
		}
		else // ThisActor != nullptr
		{
			if (LastActor != ThisActor)
			{
				// case 4
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// case 5
			}
		}
	}
}
