// Copyright 


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag SocketTag, bool bOverridePitch, float PitchOverride)
{
    const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); // 서버인 경우 true 리턴.
    if (!bIsServer)
    {
        return;
    }

    if (ProjectileClass)
    {
        // Execute_GetCombatSocketLocation: GetCombatSocketLocation 함수를 BlueprintNative로 만들어서 이 함수를 사용해야하는듯.
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
        
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation);
        SpawnTransform.SetRotation(Rotation.Quaternion());
        
        AAuraProjectile* Projectile =  GetWorld()->SpawnActorDeferred<AAuraProjectile>(
                ProjectileClass,
                SpawnTransform,
                GetOwningActorFromActorInfo(),
                Cast<APawn>(GetAvatarActorFromActorInfo()),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn
            );

        Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();
        
        Projectile->FinishSpawning(SpawnTransform);
    }
}
