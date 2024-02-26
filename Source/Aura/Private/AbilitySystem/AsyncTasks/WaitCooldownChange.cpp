// Copyright 


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
    UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
    WaitCooldownChange->ASC = AbilitySystemComponent;
    WaitCooldownChange->CooldownTag = InCooldownTag;

    if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
    {
        WaitCooldownChange->EndTask();
        return nullptr;
    }

    // To knoe when a cooldown has ended (Cooldown Tag has been removed)
    AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
        WaitCooldownChange,
        &UWaitCooldownChange::CooldownTagChanged
    );

    // To know when a cooldown effect had been applied
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
        WaitCooldownChange,
        &UWaitCooldownChange::OnActiveEffectAdded
    );

    return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
    if (!IsValid(ASC))
    {
        return;
    }

    ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

    SetReadyToDestroy();
    MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
    if (NewCount == 0)
    {
        CooldownEnd.Broadcast(0.f);
    }
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
    {
        FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
        TArray<float> TimesToRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
        if (TimesToRemaining.Num() > 0)
        {
            float TimeRemaning = TimesToRemaining[0];
            for (int32 i = 1; i < TimesToRemaining.Num(); i++)
            {
                TimeRemaning = FMath::Max(TimeRemaning, TimesToRemaining[i]);
            }
            
            CooldownStart.Broadcast(TimeRemaning);
        }
    }
}
