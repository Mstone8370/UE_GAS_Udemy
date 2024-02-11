// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
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
    
public:
    //~ Begin Enemy Interface
    virtual void HighlightActor() override;
    virtual void UnHighlightActor() override;
    //~ End Enemy Interface

    //~ Begin Combat Interface
    virtual int32 GetPlayerLevel() override;
    //~ End Combat Interface

    // For Health Bar
    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnHealthChanged;
    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
    int32 Level;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> HealthBar;
};
