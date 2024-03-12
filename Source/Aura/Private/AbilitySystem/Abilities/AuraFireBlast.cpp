// Copyright 


#include "AbilitySystem/Abilities/AuraFireBlast.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
    const float ScaledDamage = DamageScalableFloat.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    return FString::Printf(
        TEXT(
            // Title
            "<Title>FIRE BLAST</>\n\n"

            // Details
            "<Small>Level: </><Level>%d</>\n"
            "<Small>ManaCost: </><ManaCost>%.1f</>\n"
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

            // Description
            "<Default>Launches %d fire balls in all direction, each coming back and exploding upon return, causing </>"
            "<Damage>%d</><Default> radial fire damage with a chance to burn.</>\n"
        ),
        Level,
        ManaCost,
        Cooldown,
        NumFireBalls,
        FMath::FloorToInt32(ScaledDamage)
    );
}

FString UAuraFireBlast::GetLextLevelDescription(int32 Level)
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
            "<Default>Launches %d fire balls in all direction, each coming back and exploding upon return, causing </>"
            "<Damage>%d</><Default> radial fire damage with a chance to burn.</>\n"
        ),
        Level,
        ManaCost,
        Cooldown,
        NumFireBalls,
        FMath::FloorToInt32(ScaledDamage)
    );
}
