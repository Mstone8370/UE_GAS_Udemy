// Copyright 


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();

    check(AuraContext);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (IsValid(Subsystem))  // 멀티플레이어에서는 valid 하지 않을 수도 있음. 예를 들면 다른 플레이어인 경우.
    {
        Subsystem->AddMappingContext(AuraContext, 0);
    }

    SetShowMouseCursor(true);
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
    AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
    AuraInputComponent->BindAbilityActions(
        InputConfig,
        this,
        &ThisClass::AbilityInputTagPressed,
        &ThisClass::AbilityInputTagReleased,
        &ThisClass::AbilityInputTagHeld
    );
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}

void AAuraPlayerController::CursorTrace()
{
    FHitResult CursorHit;
    GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit || !IsValid(CursorHit.GetActor()))
    {
        return;
    }

    LastActor = ThisActor;
    ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

    /**
     * Line trace from cursor. There are several scenaios:
     *  A. LastActor is null && ThisActor is null
     *      - Do nothing.
     *  B. LastActor is null && ThisActor is valid
     *      - Highlight ThisActor
     *  C. LastActor is valid && ThisActor is null
     *      - UnHighlight LastActor
     *  D. Both Actors are valid, but LastActor != ThisActor
     *      - Unhighlight LastActor, and Highlight ThisActor
     *  E. Both Actors are valid, and are the same actor
     *      - Do nothing
     */
    if (LastActor != ThisActor)
    {
        if (LastActor)
        {
            LastActor->UnHighlightActor();
        }
        if (ThisActor)
        {
            ThisActor->HighlightActor();
        }
    }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    // GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (!GetASC())
    {
        return;
    }
    GetASC()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!GetASC())
    {
        return;
    }
    GetASC()->AbilityInputTagHeld(InputTag);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
    if (!IsValid(AuraAbilitySystemComponent))
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
                UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())
            );
    }
    return AuraAbilitySystemComponent;
}
