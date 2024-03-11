// Copyright 


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
{
    const float ScaledDamage = DamageScalableFloat.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    if (Level == 1)
    {
        return FString::Printf(
            TEXT(
                // Title
                "<Title>ARCANE SHARDS</>\n\n"

                // Details
                "<Small>Level: </><Level>%d</>\n"
                "<Small>ManaCost: </><ManaCost>%.1f</>\n"
                "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

                // Description
                "<Default>Summon shards of arcane energy, causing radial arcane damage of </>"
                "<Damage>%d</><Default> at the shards origin.</>\n"
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
                "<Title>ARCANE SHARDS</>\n\n"

                // Details
                "<Small>Level: </><Level>%d</>\n"
                "<Small>ManaCost: </><ManaCost>%.1f</>\n"
                "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

                // Description
                "<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"
                "<Damage>%d</><Default> at the shards origin.</>\n"
            ),
            Level,
            ManaCost,
            Cooldown,
            FMath::Min(1 + (Level - 1) * 3, 7),
            FMath::FloorToInt32(ScaledDamage)
        );
    }
}

FString UAuraArcaneShards::GetLextLevelDescription(int32 Level)
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
            "<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"
            "<Damage>%d</><Default> at the shards origin.</>\n"
        ),
        Level,
        ManaCost,
        Cooldown,
        FMath::Min(1 + (Level - 1) * 3, 7),
        FMath::FloorToInt32(ScaledDamage)
    );
}