// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

struct FWidgetControllerParams;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UAttributeSet;
class UAuraUserWidget;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

    /**
     * Overlay Widget Controller
     */
public:
    void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

    UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
    
private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
    UPROPERTY(EditAnywhere)
    TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
    
    UPROPERTY()
    TObjectPtr<UAuraUserWidget> OverlayWidget;
    UPROPERTY()
    TObjectPtr<UOverlayWidgetController> OverlayWidgetController;


    /**
     * Attribute Menu Widget Controller
     */
public:
    UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
    
private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

    UPROPERTY()
    TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
    
};
