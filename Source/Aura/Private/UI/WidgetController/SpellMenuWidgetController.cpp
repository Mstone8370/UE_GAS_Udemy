// Copyright 


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValue()
{
    BroadcastAbilityInfo();

    SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
    GetAuraASC()->AbilityStatusChanged.AddLambda(
        [this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
        {
            if (AbilityInfo)
            {
                FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
                Info.StatusTag = StatusTag;
                Info.Level = AbilityLevel;
                AbilityInfoDelegate.Broadcast(Info);
            }
        }
    );

    GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
        [this](int32 NewLevel)
        {
            SpellPointsChangedDelegate.Broadcast(NewLevel);
        }
    );
}

void USpellMenuWidgetController::SpendPointButtonPressed(const FGameplayTag& AbilityTag)
{
    GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
}
