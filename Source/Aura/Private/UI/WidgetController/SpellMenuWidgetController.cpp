// Copyright 


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

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

    GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

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

bool USpellMenuWidgetController::GetSpellDescription(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
    return GetAuraASC()->GetDecriptionsByAbilityTag(AbilityInfo, AbilityTag, OutDescription, OutNextLevelDescription);
}

void USpellMenuWidgetController::EquipButtonPressed(const FGameplayTag& AbilityTag)
{
    const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
    
    WaitForEquipDelegate.Broadcast(AbilityType);
    bWaitingForEquipSelection = true;

    const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(AbilityTag);
    if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
    {
        SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(AbilityTag);
    }
}

void USpellMenuWidgetController::StopWaitForEquipSelection()
{
    bWaitingForEquipSelection = false;
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
    if (!bWaitingForEquipSelection)
    {
        return;
    }
    // Check selected ability against the slot's ability type.
    // (don't equip on offensive spell in a passive slot vice versa)
    const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
    if (!SelectedAbilityType.MatchesTagExact(AbilityType))
    {
        return;
    }

    GetAuraASC()->ServerEquipAbility(AbilityTag, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
    bWaitingForEquipSelection = false;

    // 이미 equip한 어빌리티를 다른 슬롯에 옮기는 경우, 이전의 슬롯을 비우기 위한 AbilityInfo
    FAuraAbilityInfo LastSlotInfo;
    LastSlotInfo.InputTag = PreviousSlot;
    AbilityInfoDelegate.Broadcast(LastSlotInfo);

    // 새로 equip한 어빌리티의 슬롯에 사용되는 AbilityInfo
    FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
    Info.StatusTag = Status;
    Info.InputTag = Slot;
    AbilityInfoDelegate.Broadcast(Info);

    SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
}
