// Copyright 


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
    const float ScaledDamage = DamageScalableFloat.GetValueAtLevel(Level);
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
            FMath::FloorToInt32(ScaledDamage)
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
            FMath::FloorToInt32(ScaledDamage)
        );
    }
}

FString UAuraFireBolt::GetLextLevelDescription(int32 Level)
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
            "<Default>Launches %d bolts of fire, exploding on impact and dealing </>"
            "<Damage>%d</><Default> fire damage with a chance to burn.</>\n"
        ),
        Level,
        ManaCost,
        Cooldown,
        FMath::Min(Level, NumProjectiles),
        FMath::FloorToInt32(ScaledDamage)
    );
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
    const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
    if (!bIsServer || !ProjectileClass)
    {
        return;
    }

    const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
        GetAvatarActorFromActorInfo(),
        SocketTag
    );
    FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
    Rotation.Pitch = 0.f;
    if (bOverridePitch)
    {
        Rotation.Pitch = PitchOverride;
    }
    const FVector Forward = Rotation.Vector();

    ProjectileSpread = FMath::Clamp(ProjectileSpread, 0.f, 360.f);
    NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
    if (NumProjectiles > 1)
    {
        float DeltaSpread = 0.f;
        FVector SpreadStart = Forward;

        const bool bIsCircleSpread = FMath::IsNearlyEqual(ProjectileSpread, 360.f);
        if (bIsCircleSpread)
        {
            DeltaSpread = ProjectileSpread / NumProjectiles;
        }
        else
        {
            DeltaSpread = ProjectileSpread / (NumProjectiles - 1);
            SpreadStart = SpreadStart.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
        }

        for (int32 i = 0; i < NumProjectiles; i++)
        {
            const FVector Direction = SpreadStart.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);

            FVector Start = SocketLocation + FVector(0.f, 0.f, 10.f);
            FVector End = Start + Direction * 75.f;
            UKismetSystemLibrary::DrawDebugArrow(
                GetAvatarActorFromActorInfo(),
                Start,
                End,
                5,
                FLinearColor::Red,
                120.f,
                2.f
            );
        }
    }
    else
    {
        FVector Start = SocketLocation + FVector(0.f, 0.f, 10.f);
        FVector End = Start + Forward * 75.f;
        UKismetSystemLibrary::DrawDebugArrow(
            GetAvatarActorFromActorInfo(),
            Start,
            End,
            5,
            FLinearColor::Red,
            120.f,
            2.f
        );
    }

    /*
    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SocketLocation);
    SpawnTransform.SetRotation(Rotation.Quaternion());

    AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
        ProjectileClass,
        SpawnTransform,
        GetOwningActorFromActorInfo(),
        Cast<APawn>(GetAvatarActorFromActorInfo()),
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();

    Projectile->FinishSpawning(SpawnTransform);
    */
}
