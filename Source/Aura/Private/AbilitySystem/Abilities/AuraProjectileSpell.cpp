// Copyright 


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
    if (CombatInterface && ProjectileClass)
    {
        const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
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
        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
       
        const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
        // const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());
        const float ScaleDamage = Damage.GetValueAtLevel(10);
        
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaleDamage);
        // or
        // SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.Damage, *DAMAGE_AMOUNT*);
        
        Projectile->DamageEffectSpecHandle = SpecHandle;
        
        Projectile->FinishSpawning(SpawnTransform);
    }
}
