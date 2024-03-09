// Copyright 


#include "AbilitySystem/Abilities/AuraPassiveAbility.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
    {
        AuraASC->DeactivatePassiveAbility.AddUObject(this, &UAuraPassiveAbility::ReciveDeactivate);
    }
}

void UAuraPassiveAbility::ReciveDeactivate(const FGameplayTag& InAbilityTag)
{
    if (AbilityTags.HasTagExact(InAbilityTag))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}
