// Copyright 


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
    FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

    const float DamageMagnitude = DamageScalableFloat.GetValueAtLevel(GetAbilityLevel());
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, DamageMagnitude);

    if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
    {
        GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
    }
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefault(AActor* TargetActor) const
{
    FDamageEffectParams Params;
    Params.WorldContextObject = GetAvatarActorFromActorInfo();
    Params.DamageGameplayEffectClass = DamageEffectClass;
    Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    Params.BaseDamage = DamageScalableFloat.GetValueAtLevel(GetAbilityLevel());
    Params.AbilityLevel = GetAbilityLevel();
    Params.DamageType = DamageType;
    Params.DebuffChance = DebuffChance;
    Params.DebuffDamage = DebuffDamage;
    Params.DebuffFrequency = DebuffFrequency;
    Params.DebuffDuration = DebuffDuration;
    Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
    Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
    Params.KnockbackChance = KnockbackChance;

    if (IsValid(TargetActor))
    {
        const FVector ToTargetDirection = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();
        const FVector ImpulseAndForceDirection = (ToTargetDirection + TargetActor->GetActorUpVector()).GetSafeNormal();
        Params.DeathImpulse = ImpulseAndForceDirection * DeathImpulseMagnitude;
        if (FMath::FRandRange(0.f, 100.f) <= Params.KnockbackChance)
        {
            Params.KnockbackForce = ImpulseAndForceDirection * KnockbackForceMagnitude;
        }
    }

    if (bRadialDamage)
    {
        Params.bRadialDamage = bRadialDamage;
        Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
        Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
        Params.RadialDamageOrigin = RadialDamageOrigin;
    }
    
    return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
    return DamageScalableFloat.GetValueAtLevel(GetAbilityLevel());
}
