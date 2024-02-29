// Copyright 


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelUpInfo.h"

void UOverlayWidgetController::BroadcastInitialValue()
{
    OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
    OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
    OnManaChanged.Broadcast(GetAuraAS()->GetMana());
    OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
    
    OnPlayerLevelChangedDelegate.Broadcast(GetAuraPS()->GetPlayerLevel());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
    GetAuraPS()->OnLevelChangedDelegate.AddLambda(
        [this](int32 NewLevel)
        {
            OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
        }
    );
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnHealthChanged.Broadcast(Data.NewValue);
        }
    );
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxHealthChanged.Broadcast(Data.NewValue);
        }
    );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnManaChanged.Broadcast(Data.NewValue);
        }
    );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxManaChanged.Broadcast(Data.NewValue);
        }
    );

    GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);

    GetAuraASC()->EffectAssetTags.AddLambda(
        [this](const FGameplayTagContainer& AssetTags)
        {
            // 해당 게임플레이 태그는 Native가 아니기 때문에 c++에서는 요청을 해서 받아와야함.
            const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));

            for (const FGameplayTag& Tag : AssetTags)
            {
                // For example, say that Tag = Message.HealthPotion
                // "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchTag("Message.HealthPotion") will return False
                if (!Tag.MatchesTag(MessageTag))
                {
                    continue;
                }

                if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag))
                {
                    MessageWidgetRow.Broadcast(*Row);
                }
            }
        }
    );
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
    const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;

    checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));

    const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
    const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

    if (0 < Level && Level <= MaxLevel)
    {
        const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
        const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

        const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
        const int32 XPForThieLevel = NewXP - PreviousLevelUpRequirement;
        const float XPBarPercent = float(XPForThieLevel) / DeltaLevelRequirement;

        OnXPPrecentChangedDelegate.Broadcast(XPBarPercent);
    }
}
