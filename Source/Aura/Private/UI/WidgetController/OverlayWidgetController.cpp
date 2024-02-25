// Copyright 


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

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
        if (AuraASC->bStartupAbilitiesGiven)
        {
            // 일반적인 경우.
            // 서버에서 먼저 startup 어빌리티를 캐릭터에 추가했음.
            // startup 어빌리티가 추가된 다음에 호출되어야 할 함수를 바로 호출.
            OnInitializeStartupAbilities(AuraASC);
        }
        else
        {
            // 클라이언트에서 먼저 여기에 도착한 경우.
            // 서버에서 캐릭터에 startup 어빌리티를 추가하기를 기다림.
            AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
        }

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
    // TODO: Get information about all given abilities, loop up their Abiltiy Info, and boradcast it to widget.
    if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven)
    {
        return;
    }

    
}
