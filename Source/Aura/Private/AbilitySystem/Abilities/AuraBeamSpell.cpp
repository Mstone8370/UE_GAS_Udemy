// Copyright 


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
    if (HitResult.bBlockingHit)
    {
        MouseHitLocation = HitResult.ImpactPoint;
        MouseHitActor = HitResult.GetActor();
    }
    else
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, GetCurrentActivationInfo(), true);
    }
}

void UAuraBeamSpell::StoreOwnerVariables()
{
    if (CurrentActorInfo)
    {
        if (CurrentActorInfo->PlayerController.IsValid())
        {
            OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
        }
        if (CurrentActorInfo->AvatarActor.IsValid())
        {
            OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
        }
    }
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
    check(OwnerCharacter);
    if (OwnerCharacter->Implements<UCombatInterface>())
    {
        if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
        {
            TArray<AActor*> ActorsToIgnore;
            ActorsToIgnore.Add(OwnerCharacter);

            const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));

            FHitResult HitResult;
            UKismetSystemLibrary::SphereTraceSingle(
                OwnerCharacter,
                SocketLocation,
                BeamTargetLocation,
                10.f,
                TraceTypeQuery1,
                false,
                ActorsToIgnore,
                EDrawDebugTrace::None,
                HitResult,
                true
            );

            if (HitResult.bBlockingHit)
            {
                MouseHitLocation = HitResult.ImpactPoint;
                MouseHitActor = HitResult.GetActor();
            }
        }
    }

    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
    {
        if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
        {
            CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
        }
    }
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
    OutAdditionalTargets.Empty();

    if (!MouseHitActor->Implements<UCombatInterface>())
    {
        return;
    }

    TArray<AActor*> ActorsToIngore;
    ActorsToIngore.Add(GetAvatarActorFromActorInfo());
    ActorsToIngore.Add(MouseHitActor);
    TArray<AActor*> OverlappingActors;
    const FVector Origin = MouseHitActor->GetActorLocation();
    UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
        GetAvatarActorFromActorInfo(),
        OverlappingActors,
        ActorsToIngore,
        850.f,
        Origin
    );

    int32 NumAdditionalTargets = FMath::Min(MaxNumShockTargets, GetAbilityLevel() - 1);
    UAuraAbilitySystemLibrary::GetClosestTarget(
        NumAdditionalTargets,
        OverlappingActors,
        OutAdditionalTargets,
        Origin
    );

    for (AActor* Target : OutAdditionalTargets)
    {
        if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
        {
            if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
            {
                CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
            }
        }
    }
}

void UAuraBeamSpell::UnbindAllDelegate(TArray<AActor*>& AdditionalTargets)
{
    if (IsValid(MouseHitActor))
    {
        if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
        {
            CombatInterface->GetOnDeathDelegate().RemoveAll(this);
        }
    }

    for (AActor* Target : AdditionalTargets)
    {
        if (IsValid(Target))
        {
            if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
            {
                CombatInterface->GetOnDeathDelegate().RemoveAll(this);
            }
        }
    }
}
