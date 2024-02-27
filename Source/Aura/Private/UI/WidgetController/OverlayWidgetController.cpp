// Copyright 


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelUpInfo.h"

void UOverlayWidgetController::BroadcastInitialValue()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
    
    OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
    OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
    OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
    AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
    
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnHealthChanged.Broadcast(Data.NewValue);
        }
    );
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxHealthChanged.Broadcast(Data.NewValue);
        }
    );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnManaChanged.Broadcast(Data.NewValue);
        }
    );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxManaChanged.Broadcast(Data.NewValue);
        }
    );

    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
    {
        AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);

        AuraASC->EffectAssetTags.AddLambda(
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
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
    if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven)
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
            AbilityInfoDelegate.Broadcast(Info);
        }
    );
    AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
    const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
    const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;

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
