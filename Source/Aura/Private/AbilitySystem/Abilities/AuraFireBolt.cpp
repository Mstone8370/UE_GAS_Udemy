// Copyright 


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

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

    bool bIsHomingTargetValid = false;
    if (IsValid(HomingTarget) && HomingTarget->Implements<UCombatInterface>())
    {
        bIsHomingTargetValid = true;
    }

    const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
        GetAvatarActorFromActorInfo(),
        SocketTag
    );
    FRotator ForwardRotation = (ProjectileTargetLocation - SocketLocation).Rotation();
    ForwardRotation.Pitch = 0.f;
    if (bOverridePitch)
    {
        ForwardRotation.Pitch = PitchOverride;
    }

    ProjectileSpread = FMath::Clamp(ProjectileSpread, 0.f, 360.f);
    NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());

    TArray<FVector> Directions;
    UAuraAbilitySystemLibrary::GetEvenlyRotatedVectors(
        ForwardRotation.Vector(), ProjectileSpread, NumProjectiles, FVector::UpVector, Directions
    );

    for (const FVector& Direction : Directions)
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation);
        SpawnTransform.SetRotation(Direction.Rotation().Quaternion());

        APawn* Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
        AController* Owner = Instigator->GetController();

        AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
            ProjectileClass,
            SpawnTransform,
            Owner,
            Instigator,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();

        if (bLaunchHomingProjectiles)
        {
            if (bIsHomingTargetValid)
            {
                Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
            }
            else
            {
                Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(Projectile);
                Projectile->HomingTargetSceneComponent->RegisterComponent();
                Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
                Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
                Projectile->bIsHomingToSceneComp = true;
                Projectile->HomingSceneCompLocation = ProjectileTargetLocation;
            }
            Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
            Projectile->ProjectileMovement->bIsHomingProjectile = true;
        }

        Projectile->FinishSpawning(SpawnTransform);
    }
}
