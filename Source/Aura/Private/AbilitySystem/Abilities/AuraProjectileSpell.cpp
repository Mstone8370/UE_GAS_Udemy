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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
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
            FAuraGameplayTags::Get().Montage_Attack_Weapon
        );
        FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
        Rotation.Pitch = 0.f;
        
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation);
        SpawnTransform.SetRotation(Rotation.Quaternion());
        
        AAuraProjectile* Projectile =  GetWorld()->SpawnActorDeferred<AAuraProjectile>(
                ProjectileClass,
                SpawnTransform,
                GetOwningActorFromActorInfo(),
                Cast<APawn>(GetOwningActorFromActorInfo()),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn
            );
        
        const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

        // Effect Context 설정
        FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
        EffectContextHandle.SetAbility(this);
        EffectContextHandle.AddSourceObject(Projectile);
        TArray<TWeakObjectPtr<AActor>> Actors;
        Actors.Add(Projectile);
        EffectContextHandle.AddActors(Actors);
        FHitResult HitResult;
        HitResult.Location = ProjectileTargetLocation;
        EffectContextHandle.AddHitResult(HitResult);
        // Effect Context 설정 끝

        // Damage 설정
        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
        for (auto& Pair : DamageTypes)
        {
            const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
            
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
            // or
            // SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, ScaleDamage);
        }
        Projectile->DamageEffectSpecHandle = SpecHandle;
        // Damage 설정 끝
        
        Projectile->FinishSpawning(SpawnTransform);
    }
}
