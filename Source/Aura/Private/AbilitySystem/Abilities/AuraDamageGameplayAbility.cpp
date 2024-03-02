// Copyright 


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
    FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

    for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
    {
        const float DamageMagnitude = Pair.Value.GetValueAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, DamageMagnitude);
    }

    if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
    {
        GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
    }
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType)
{
    checkf(DamageTypes.Contains(DamageType), TEXT("GameplayAbility %s does not contain DamageType %s"), *GetNameSafe(this), *DamageType.ToString());
    return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}
