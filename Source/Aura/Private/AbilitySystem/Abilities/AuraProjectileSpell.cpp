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
    if (!bIsServer || !ProjectileClass)
    {
        return;
    }

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
    /*
    * 데미지가 전달되는 전체 과정
    * 
    *  1. DamageEffectParams 구조체에 이 어빌리티의 추가 정보를 넣어서 Projectile에 저장.
    *  
    *  2. Projectile이 적과 오버랩되서 데미지를 주어야할 때 DamageEffectParams에 추가정보를 넣음.
    *  
    *  3. UAuraAbilitySystemLibrary에서 DamageEffectParams의 정보를 통해 GameplayEffect를 적용함.
    *  
    *  4. GameplayEffect를 적용하기 전에 GameplayEffect에 DamageEffectParams의 정보를 통해
    *     SetByCallerMagnitude 값을 추가하고, 임펄스를 설정함.
    *  
    *  5. GameplayEffect를 적용하고, DamageEffect는 데미지 값을 계산할 때 ExecCalc_Damage를 사용함.
    *  
    *  6. ExecCalc_Damage에서 Target의 Attribute와 SetByCallerMagnitude의 값을 통해 
    *     데미지, 치명타, 디버프 등을 결정함.
    *  
    *  7. 결정된 값은 FAuraGamepleyEffectContext의 각 변수에 저장되어 전달됨.
    *  
    *  8. -- 여기까지는 Source는 데미지를 준 액터, Target은 데미지를 받은 액터 --
    * 
    *  9. ExecCalc_Damage에서 결정된 값을 통해 TargetActor의 AttributeSet의 Attribute가 조정됨.
    * 
    * 10. AttributeSet의 PostGameplayEffectExecute가 실행되고
    *     전달받은 Context인 FAuraGamepleyEffectContext를 통해 반응을 결정함.
    * 
    * DamageEffectParams --> GameplayEffect --> FAuraGamepleyEffectContext --> TargetActor
    */
        
    Projectile->FinishSpawning(SpawnTransform);
}
