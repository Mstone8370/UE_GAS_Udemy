// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValue() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|SpellPoint")
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	bool GetSpellDescription(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void StopWaitForEquipSelection();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	bool bWaitingForEquipSelection = false;

private:
	FGameplayTag SelectedSlot;
};
