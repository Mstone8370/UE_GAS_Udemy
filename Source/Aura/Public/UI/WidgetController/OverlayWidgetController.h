// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UAbilityInfo;
class UAuraUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag MessageTag = FGameplayTag();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Message = FText();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UAuraUserWidget> MessageWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, const FUIWidgetRow, Row);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnAttributeChangedSignature OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnAttributeChangedSignature OnMaxHealthChanged;
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnAttributeChangedSignature OnManaChanged;
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnAttributeChangedSignature OnMaxManaChanged;
    UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
    FMessageWidgetRowSignature MessageWidgetRow;
    
    virtual void BroadcastInitialValue() override;
    virtual void BindCallbacksToDependencies() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
    TObjectPtr<UDataTable> MessageWidgetDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
    TObjectPtr<UAbilityInfo> AbilityInfo;

    template<typename T>
    T* GetDataTableRowByTag(const UDataTable* DataTable, const FGameplayTag& Tag) const;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(const UDataTable* DataTable, const FGameplayTag& Tag) const
{
    if (!IsValid(DataTable))
    {
        return nullptr;
    }
    return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
