// Copyright 


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widgets/DamageTextComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/MagicCircle.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;

    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
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
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
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
    AutoRun();
    UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
    if (MagicCircleClass && !IsValid(MagicCircle))
    {
        MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
        if (DecalMaterial)
        {
            MagicCircle->SetMaterial(DecalMaterial);
        }
    }
}

void AAuraPlayerController::HideMagicCircle()
{
    if (IsValid(MagicCircle))
    {
        MagicCircle->Destroy();
    }
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float DamageAmount, ACharacter* TargetCharacter, const bool bBlockedHit, const bool bCriticalHit)
{
    if (!IsLocalController())
    {
        return;
    }
    
    if (IsValid(TargetCharacter) && DamageTextComponentClass)
    {
        UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
        DamageText->RegisterComponent();
        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
    }
}

void AAuraPlayerController::AutoRun()
{
    if (!bAutoRunning)
    {
        return;
    }
    
    if (APawn* ControlledPawn = GetPawn())
    {
        // 주어진 위치에서 스플라인중의 가장 가까운 한 점의 위치를 리턴함. 스플라인의 포인트가 아니라 선을 구성하는 하나의 점이라는 것을 주의.
        const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
        // 주어진 위치에서 스플라인중의 가장 가까운 한 점의 방향을 리턴함. 스플라인의 포인트가 아니라 선을 구성하는 하나의 점이라는 것을 주의.
        const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
        // 이런 방식으로는 방향의 기준이 Pawn이 아니라 스플라인이므로 Pawn이 스플라인 위에 있지 않다면 목표 위치로 향하지 않음.
        ControlledPawn->AddMovementInput(Direction);

        const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
        if (DistanceToDestination <= AutoRunAcceptanceRadius)
        {
            bAutoRunning = false;
        }
    }
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
    {
        return;
    }

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
    if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
    {
        if (LastActor)
        {
            LastActor->UnHighlightActor();
            LastActor = nullptr;
        }
        if (ThisActor)
        {
            ThisActor->UnHighlightActor();
            ThisActor = nullptr;
        }

        return;
    }

    GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit || !IsValid(CursorHit.GetActor()))
    {
        return;
    }

    LastActor = ThisActor;
    ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
    
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

void AAuraPlayerController::UpdateMagicCircleLocation()
{
    if (IsValid(MagicCircle) && CursorHit.bBlockingHit)
    {
        MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
    }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
    {
        return;
    }

    if (GetASC())
    {
        GetASC()->AbilityInputTagPressed(InputTag);
    }

    if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        bTargeting = (ThisActor != nullptr);
        bAutoRunning = false;
    }
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
    {
        return;
    }

    if (GetASC())
    {
        GetASC()->AbilityInputTagReleased(InputTag);
    }
    
    if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) && !bTargeting && !bShiftKeyDown)
    {
        const APawn* ControlledPawn = GetPawn();
        if (FollowTime <= ShortPressThreshold && ControlledPawn)
        {
            if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
            {
                Spline->ClearSplinePoints();
                if (!NavPath->PathPoints.IsEmpty())
                {
                    for (const FVector& PointLoc : NavPath->PathPoints)
                    {
                        Spline->AddSplineWorldPoint(PointLoc);
                    }
                    CachedDestination = NavPath->PathPoints.Last();
                    bAutoRunning = true;
                }
            }
            if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
            }
        }
    
        FollowTime = 0.f;
        bTargeting = false;
    }
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
    {
        return;
    }

    if (bTargeting || bShiftKeyDown || !InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagHeld(InputTag);
        }
        return;
    }
    
    FollowTime += GetWorld()->GetDeltaSeconds();

    if (CursorHit.bBlockingHit)
    {
        CachedDestination = CursorHit.ImpactPoint;
    }

    if (APawn* ControlledPawn = GetPawn())
    {
        const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
        ControlledPawn->AddMovementInput(WorldDirection);
    }
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
