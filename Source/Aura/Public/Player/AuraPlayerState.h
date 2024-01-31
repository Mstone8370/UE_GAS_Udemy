// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
    AAuraPlayerState();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
    FORCEINLINE int32 GetPlayerLevel() const { return Level; }

private:
    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
    int32 Level;

    UFUNCTION()
    void OnRep_Level(int32 OldLevel);
};
