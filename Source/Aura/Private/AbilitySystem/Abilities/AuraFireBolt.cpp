// Copyright 


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
    const float Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    if (Level == 1)
    {
        return FString::Printf(
            TEXT(
                // Title
                "<Title>FIRE BOLT</>\n\n"

                // Details
                "<Small>Level: </><Level>%d</>\n"
                "<Small>ManaCost: </><ManaCost>%.1f</>\n"
                "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

                // Description
                "<Default>Launches a bolt of fire, exploding on impact and dealing </>"
                "<Damage>%d</><Default> fire damage with a chance to burn.</>\n"
            ),
            Level,
            ManaCost,
            Cooldown,
            FMath::FloorToInt32(Damage)
        );
    }
    else
    {
        return FString::Printf(
            TEXT(
                // Title
                "<Title>FIRE BOLT</>\n\n"

                // Details
                "<Small>Level: </><Level>%d</>\n"
                "<Small>ManaCost: </><ManaCost>%.1f</>\n"
                "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

                // Description
                "<Default>Launches %d bolts of fire, exploding on impact and dealing </>"
                "<Damage>%d</><Default> fire damage with a chance to burn.</>\n"
            ),
            Level,
            ManaCost,
            Cooldown,
            FMath::Min(Level, NumProjectiles),
            FMath::FloorToInt32(Damage)
        );
    }
}

FString UAuraFireBolt::GetLextLevelDescription(int32 Level)
{
    const float Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
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
            "<Default>Launches %d bolts of fire, exploding on impact and dealing </>"
            "<Damage>%d</><Default> fire damage with a chance to burn.</>\n"
        ),
        Level,
        ManaCost,
        Cooldown,
        FMath::Min(Level, NumProjectiles),
        FMath::FloorToInt32(Damage)
    );
}
