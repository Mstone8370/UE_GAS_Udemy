// Copyright 


#include "AbilitySystem/Abilities/AuraElectrocute.h"

FString UAuraElectrocute::GetDescription(int32 Level)
{
    const float ScaledDamage = DamageScalableFloat.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    if (Level == 1)
    {
        return FString::Printf(
            TEXT(
                // Title
                "<Title>ELECTROCUTE</>\n\n"

                // Details
                "<Small>Level: </><Level>%d</>\n"
                "<Small>ManaCost: </><ManaCost>%.1f</>\n"
                "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

                // Description
                "<Default>Emits a beam of lightning, connecting with the target, repeatdly causing </>"
                "<Damage>%d</><Default> lightning damage with a chance to stun.</>\n"
            ),
            Level,
            ManaCost,
            Cooldown,
            FMath::FloorToInt32(ScaledDamage)
        );
    }
    else
    {
        return FString::Printf(
            TEXT(
                // Title
                "<Title>ELECTROCUTE</>\n\n"

                // Details
                "<Small>Level: </><Level>%d</>\n"
                "<Small>ManaCost: </><ManaCost>%.1f</>\n"
                "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

                // Description
                "<Default>Emits a beam of lightning, connecting with the target, propagating to %d additional targets nearby, repeatdly causing </>"
                "<Damage>%d</><Default> lightning damage with a chance to stun.</>\n"
            ),
            Level,
            ManaCost,
            Cooldown,
            FMath::Min(MaxNumShockTargets, Level - 1),
            FMath::FloorToInt32(ScaledDamage)
        );
    }
}

FString UAuraElectrocute::GetLextLevelDescription(int32 Level)
{
    const float ScaledDamage = DamageScalableFloat.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    return FString::Printf(
        TEXT(
            // Title
            "<Title>NEXT LEVEL:</>\n\n"

            // Details
            "<Small>Level: </><Level>%d</>\n"
            "<Small>ManaCost: </><ManaCost>%.1f</>\n"
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

            // Description
            "<Default>Emits a beam of lightning, connecting with the target, propagating to %d additional targets nearby, repeatdly causing </>"
            "<Damage>%d</><Default> lightning damage with a chance to stun.</>\n"
        ),
        Level,
        ManaCost,
        Cooldown,
        FMath::Min(MaxNumShockTargets, Level - 1),
        FMath::FloorToInt32(ScaledDamage)
    );
}