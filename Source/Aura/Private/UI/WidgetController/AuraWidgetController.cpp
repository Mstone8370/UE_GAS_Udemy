// Copyright 


#include "UI/WidgetController/AuraWidgetController.h"

#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
    PlayerController = WCParams.PlayerController;
    PlayerState = WCParams.PlayerState;
    AbilitySystemComponent = WCParams.AbilitySystemComponent;
    AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValue() {}

void UAuraWidgetController::BindCallbacksToDependencies() {}

void UAuraWidgetController::BroadcastAbilityInfo()
{
    if (!GetAuraASC()->bStartupAbilitiesGiven)
    {
        return;
    }

    FForEachAbility BroadcastDelegate;
    BroadcastDelegate.BindLambda(
        [this](const FGameplayAbilitySpec& AbilitySpec)
        {
            check(AbilityInfo);
            FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);
            FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
            Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
            Info.StatusTag = UAuraAbilitySystemComponent::GetStatusFromSpec(AbilitySpec);
            AbilityInfoDelegate.Broadcast(Info);
        }
    );
    GetAuraASC()->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAurePC()
{
    if (!AuraPlayerController)
    {
        AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
    }
    return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
    if (!AuraPlayerState)
    {
        AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
    }
    return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
    if (!AuraAbilitySystemComponent)
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
    }
    return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
    if (!AuraAttributeSet)
    {
        AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
    }
    return AuraAttributeSet;
}
