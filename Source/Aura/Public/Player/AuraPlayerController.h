// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AAuraPlayerController();
    
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

public:
    virtual void PlayerTick(float DeltaTime) override;

private:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> AuraContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UAuraInputConfig> InputConfig;

    UPROPERTY()
    TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
    
    FHitResult CursorHit;
    
    IEnemyInterface* LastActor;
    IEnemyInterface* ThisActor;

    // Begin Click To Move variables
    float FollowTime = 0.f;
    float ShortPressThreshold = 0.5f;
    bool bAutoRunning = false;
    bool bTargeting = false; // is cursor targeting something
    FVector CachedDestination = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USplineComponent> Spline;
    
    UPROPERTY(EditDefaultsOnly)
    float AutoRunAcceptanceRadius = 50.f;
    // End Click To Move variables
    
    void AutoRun();
    void Move(const FInputActionValue& InputActionValue);

    void CursorTrace();

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);


    UAuraAbilitySystemComponent* GetASC();
};
