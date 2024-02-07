// Copyright 


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    const bool bIsServer = HasAuthority(&ActivationInfo); // 서버인 경우 true 리턴.
    if (!bIsServer)
    {
        return;
    }

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
    if (CombatInterface && ProjectileClass)
    {
        const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
        
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation);
        // TODO: Set the Projectile Rotation.
        const FRotator AvatarRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
        const FRotator SpawnRotation(0.f, AvatarRotation.Yaw, 0.f);
        SpawnTransform.SetRotation(SpawnRotation.Quaternion());
        
        AAuraProjectile* Projectile =  GetWorld()->SpawnActorDeferred<AAuraProjectile>(
                ProjectileClass,
                SpawnTransform,
                GetOwningActorFromActorInfo(),
                Cast<APawn>(GetOwningActorFromActorInfo()),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn
            );
        // TODO: Give the Projectile a Gameplay Effect Spec for causing Damage.
        Projectile->FinishSpawning(SpawnTransform);
    }
}
