// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
class USplineComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
class UNiagaraSystem;

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

    UFUNCTION(Client, Reliable)
    void ShowDamageNumber(const float DamageAmount, ACharacter* TargetCharacter, const bool bBlockedHit, const bool bCriticalHit);

private:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> AuraContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ShiftAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UAuraInputConfig> InputConfig;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
    
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

    FORCEINLINE void ShiftPressed() { bShiftKeyDown = true; }
    FORCEINLINE void ShiftReleased() { bShiftKeyDown = false; }
    bool bShiftKeyDown = false;

    void CursorTrace();

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);

    UAuraAbilitySystemComponent* GetASC();
};
