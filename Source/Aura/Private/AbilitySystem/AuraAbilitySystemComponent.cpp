// Copyright 


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    // on Server

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
            AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
            GiveAbility(AbilitySpec);
        }
    }
    bStartupAbilitiesGiven = true;
    AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
    // on Server

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupPassiveAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    FScopedAbilityListLock ActiveScopeLock(*this);

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        bool bIsInputTagAbility = AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag);
        bool bIsEquippedAbility = AbilitySpec.DynamicAbilityTags.HasTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped);
        if (bIsInputTagAbility && bIsEquippedAbility)
        {
            AbilitySpecInputPressed(AbilitySpec); // Ability가 Input Pressed임을 알려주기기만 하는 목적?
            if (AbilitySpec.IsActive())
            {
                InvokeReplicatedEvent(
                    EAbilityGenericReplicatedEvent::InputPressed,
                    AbilitySpec.Handle,
                    AbilitySpec.ActivationInfo.GetActivationPredictionKey()
                );
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    FScopedAbilityListLock ActiveScopeLock(*this);

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        bool bIsInputTagAbility = AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag);
        bool bIsEquippedAbility = AbilitySpec.DynamicAbilityTags.HasTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped);
        if (bIsInputTagAbility && bIsEquippedAbility)
        {
            AbilitySpecInputPressed(AbilitySpec); // Ability가 Input Pressed임을 알려주기기만 하는 목적?
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    FScopedAbilityListLock ActiveScopeLock(*this);

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
        {
            AbilitySpecInputReleased(AbilitySpec); // Ability가 Input Released임을 알려주기기만 하는 목적?
            
            InvokeReplicatedEvent(
                EAbilityGenericReplicatedEvent::InputReleased,
                AbilitySpec.Handle,
                AbilitySpec.ActivationInfo.GetActivationPredictionKey()
            );
        }
    }
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
    // 아래 for 루프가 도는 동안 어빌리티에 변경사항이 적용되지 않도록 이 scpoe 동안에는 lock해둠.
    FScopedAbilityListLock ActiveScopeLock(*this);
    
    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!Delegate.ExecuteIfBound(AbilitySpec))
        {
            UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
        }
    }
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    if (AbilitySpec.Ability)
    {
        for (FGameplayTag Tag : AbilitySpec.Ability->AbilityTags)
        {
            if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
            {
                return Tag;
            }
        }
    }

    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
    {
        if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
    {
        if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
    if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
    {
        return GetStatusFromSpec(*Spec);
    }
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
    if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
    {
        return GetInputTagFromSpec(*Spec);
    }
    return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
    // 아래 for 루프가 도는 동안 어빌리티에 변경사항이 적용되지 않도록 이 scpoe 동안에는 lock해둠.
    FScopedAbilityListLock ActiveScopeLock(*this);

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        for (const FGameplayTag& Tag : AbilitySpec.Ability->AbilityTags)
        {
            if (Tag.MatchesTagExact(AbilityTag))
            {
                return &AbilitySpec;
            }
        }
    }
    return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
        if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
        {
            ServerUpgradeAttribute(AttributeTag);
        }
    }
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
    const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
    for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
    {
        if (!Info.AbilityTag.IsValid())
        {
            continue;
        }
        if (Level < Info.LevelRequirement)
        {
            continue;
        }

        if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
        {
            // 현재 가지고있지 않은 어빌리티만 확인
            FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
            AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
            GiveAbility(AbilitySpec);
            MarkAbilitySpecDirty(AbilitySpec); // 변경점을 클라이언트에게 바로 알려주게 함.
            ClientUpdateAbilityState(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
        }
    }
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
    FGameplayEventData Payload;
    Payload.EventTag = AttributeTag;
    Payload.EventMagnitude = 1;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
        IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
    }
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
    // Check Spell Point
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
        if (IPlayerInterface::Execute_GetSpellPoints(GetAvatarActor()) < 1)
        {
            return;
        }
    }

    if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
    {
        const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
        FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
        if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
        {
            // Invalid Request
            return;
        }

        FGameplayTag NewStatus = FGameplayTag();
        if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
        {
            // Unlock Ability
            NewStatus = GameplayTags.Abilities_Status_Unlocked;
            AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
            AbilitySpec->DynamicAbilityTags.AddTag(NewStatus);
            AbilitySpec->Level = 1;
        }
        else
        {
            // Upgrade Ability
            NewStatus = Status;
            ++AbilitySpec->Level;
        }
        // Update Ability State
        ClientUpdateAbilityState(AbilityTag, NewStatus, AbilitySpec->Level);
        MarkAbilitySpecDirty(*AbilitySpec);

        // Consume Spell Point
        if (GetAvatarActor()->Implements<UPlayerInterface>())
        {
            IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
        }
    }
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
    if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag)) // Locked 어빌리티인 경우 nullptr
    {
        const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
        const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

        FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
        // Equipped or Unlocked 어빌리티인 경우 Valid
        const bool bStatusValid = !Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible);
        if (bStatusValid)
        {
            bool bIsPassiveAbility = AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Passive")));

            // Equip 위치인 InputTag(Slot) 태그를 가지고있는 어빌리티들에서 InputTag(Slot) 제거
            ClearAbilitiesOfSlot(Slot, bIsPassiveAbility);
            // 이 어빌리티가 가지고있는 InputTag(Slot) 제거
            ClearSlot(AbilitySpec, bIsPassiveAbility);

            // 이 어빌리티에 InputTag(Slot) 추가
            AbilitySpec->DynamicAbilityTags.AddTag(Slot);
            // ClearAbilitiesOfSlot 함수에서 이 어빌리티의 Status가 변경되었을수도 있음
            const FGameplayTag& NewStatus = GetStatusFromSpec(*AbilitySpec);
            if (NewStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
            {
                AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);
                AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
                // Unlocked와 Equipped는 UI에 똑같이 보이므로 보기에는 차이 없음.
                ClientUpdateAbilityState(AbilityTag, GameplayTags.Abilities_Status_Equipped, AbilitySpec->Level);
            }

            // 패시브면 활성화
            if (bIsPassiveAbility)
            {
                TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
            }

            MarkAbilitySpecDirty(*AbilitySpec);
        }
        ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
    }
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
    AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec, bool InIsPassiveAbility)
{
    const FGameplayTag& Slot = GetInputTagFromSpec(*Spec);
    if (Slot.IsValid())
    {
        Spec->DynamicAbilityTags.RemoveTag(Slot);
        FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
        if (Spec->DynamicAbilityTags.HasAnyExact(FGameplayTagContainer(GameplayTags.Abilities_Status_Equipped)))
        {
            Spec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Equipped);
            Spec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);

            if (InIsPassiveAbility)
            {
                DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*Spec));
            }

            ClientUpdateAbilityState(GetAbilityTagFromSpec(*Spec), GameplayTags.Abilities_Status_Unlocked, 1);
        }
        // 어빌리티 Equip할때는 ClearSlot 함수가 여러번 사용되므로 바로바로 변경사항을 적용시켜야 꼬이지 않음.
        MarkAbilitySpecDirty(*Spec);
    }
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot, bool InIsPassiveAbility)
{
    FScopedAbilityListLock ActiveScopeLock(*this);

    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        if (AbilityHasSlot(&Spec, Slot))
        {
            ClearSlot(&Spec, InIsPassiveAbility);
        }
    }
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
    return Spec->DynamicAbilityTags.HasAnyExact(FGameplayTagContainer(Slot));

    // or

    for (FGameplayTag Tag : Spec->DynamicAbilityTags)
    {
        if (Tag.MatchesTagExact(Slot))
        {
            return true;
        }
    }
    return false;
}

bool UAuraAbilitySystemComponent::GetDecriptionsByAbilityTag(const UAbilityInfo* AbilityInfo, const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
    if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
    {
        if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
        {
            OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
            OutNextLevelDescription = AuraAbility->GetLextLevelDescription(AbilitySpec->Level + 1);

            return true;
        }
    }

    OutDescription = FString();
    OutNextLevelDescription = FString();
    
    // if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
    if (AbilityInfo)
    {
        OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
    }

    return false;
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    
    EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityState_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
    AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
    Super::OnRep_ActivateAbilities();

    // on Client

    if (!bStartupAbilitiesGiven)
    {
        bStartupAbilitiesGiven = true;
        AbilitiesGivenDelegate.Broadcast();
    }
}
