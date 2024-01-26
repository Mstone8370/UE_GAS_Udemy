// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

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

    TObjectPtr<IEnemyInterface> LastActor;
    TObjectPtr<IEnemyInterface> ThisActor;
    
    void Move(const FInputActionValue& InputActionValue);

    void CursorTrace();
};
