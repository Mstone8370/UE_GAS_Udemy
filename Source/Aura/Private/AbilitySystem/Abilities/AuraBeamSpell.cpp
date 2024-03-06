// Copyright 


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

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

void UAuraBeamSpell::StoreOwnerPlayerController()
{
    if (CurrentActorInfo && CurrentActorInfo->PlayerController.IsValid())
    {
        OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
    }
}