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

#include "Net/UnrealNetwork.h"

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

void AAuraProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAuraProjectile, HomingParam);
}

void AAuraProjectile::ServerGetHomingTarget_Implementation()
{
    FHomingParam NewParam = FHomingParam();
    NewParam.bHoming = ProjectileMovement->bIsHomingProjectile;
    NewParam.HomingAcceleration = ProjectileMovement->HomingAccelerationMagnitude;
    NewParam.bIsHomingToSceneComp = bIsHomingToSceneComp;
    NewParam.HomingSceneCompLocation = HomingSceneCompLocation;
    NewParam.HomingTarget = ProjectileMovement->HomingTargetComponent.Get();
    HomingParam = NewParam;
}

void AAuraProjectile::OnRep_HomingParam(FHomingParam OldHomingParam)
{
    ProjectileMovement->bIsHomingProjectile = HomingParam.bHoming;
    ProjectileMovement->HomingAccelerationMagnitude = HomingParam.HomingAcceleration;

    if (HomingParam.bIsHomingToSceneComp)
    {
        ProjectileMovement->HomingTargetComponent = NewObject<USceneComponent>(this);
        ProjectileMovement->HomingTargetComponent->RegisterComponent();
        ProjectileMovement->HomingTargetComponent->SetWorldLocation(HomingParam.HomingSceneCompLocation);
    }
    else
    {
        ProjectileMovement->HomingTargetComponent = HomingParam.HomingTarget;
    }

    HomingParam = FHomingParam();
}

void AAuraProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(LifeSpan);
    
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

    LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

    if (!HasAuthority())
    {
        ServerGetHomingTarget();
    }
}

void AAuraProjectile::Destroyed()
{
    if (IsValid(LoopingSoundComponent))
    {
        LoopingSoundComponent->Stop();
        LoopingSoundComponent->DestroyComponent();
        LoopingSoundComponent = nullptr;
    }

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
            DamageEffectParams.DeathImpulse = DamageEffectParams.DeathImpulseMagnitude * GetActorForwardVector();
            DamageEffectParams.TargetAbilitySystemComponent = TargetASC;

            const bool bKnockback = FMath::FRandRange(0.f, 100.f) < DamageEffectParams.KnockbackChance;
            if (bKnockback)
            {
                const FVector KnockbackDirection = (GetActorForwardVector() + OtherActor->GetActorUpVector()).GetSafeNormal();
                DamageEffectParams.KnockbackForce = DamageEffectParams.KnockbackForceMagnitude * KnockbackDirection;
            }

            UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
        }
        
        Destroy();
    }
}

