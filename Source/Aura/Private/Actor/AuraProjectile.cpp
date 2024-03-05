// Copyright 


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

AAuraProjectile::AAuraProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(Sphere);
    Sphere->SetCollisionObjectType(ECC_Projectile);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 550.f;
    ProjectileMovement->MaxSpeed = 550.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(LifeSpan);
    
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

    LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
    if (!bHit && !HasAuthority())
    {
        // 클라이언트에서 오버랩 되기 전에 Destroy된 경우.
        OnHit();
    }
    
    Super::Destroyed();
}

void AAuraProjectile::OnHit()
{
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
    if (IsValid(LoopingSoundComponent))
    {
        LoopingSoundComponent->Stop();
        LoopingSoundComponent->DestroyComponent();
        LoopingSoundComponent = nullptr;
    }
    bHit = true;
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // TODO: 서버에서 오버랩을 처리하고 게임플레이 이펙트를 적용함.
    // 그런데 지연시간이 있어서 클라이언트에서는 오버랩 이벤트가 여러번 발생하게 됨.

    if (GetInstigator() == OtherActor || !UAuraAbilitySystemLibrary::IsEnemy(GetInstigator(), OtherActor))
    {
        return;
    }

    if (!bHit)
    {
        OnHit();
    }
    
    if (HasAuthority())
    {
        if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
        {
            const FVector DeathImpulse = DamageEffectParams.DeathImpulse * GetActorForwardVector();
            DamageEffectParams.DeathImpulse = DeathImpulse;
            DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
            UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
        }
        
        Destroy();
    }
}

