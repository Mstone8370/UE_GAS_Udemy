// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UWidgetComponent;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
    AAuraEnemy();

protected:
    virtual void BeginPlay() override;
    virtual void InitAbilityActorInfo() override;
    virtual void InitializeDefaultAttributes() const override;
    
public:
    //~ Begin Enemy Interface
    virtual void HighlightActor() override;
    virtual void UnHighlightActor() override;
    virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
    virtual AActor* GetCombatTarget_Implementation() const override;
    //~ End Enemy Interface

    //~ Begin Combat Interface
    virtual int32 GetPlayerLevel() override;
    virtual void Die() override;
    //~ End Combat Interface

    virtual void PossessedBy(AController* NewController) override;

    // For Health Bar
    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnHealthChanged;
    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnMaxHealthChanged;
    
    void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bHitReacting;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float BaseWalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float LifeSpan;

    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> CombatTarget;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
    int32 Level;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> HealthBar;

    UPROPERTY(EditAnywhere, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY()
    TObjectPtr<AAuraAIController> AuraAIController;
};
