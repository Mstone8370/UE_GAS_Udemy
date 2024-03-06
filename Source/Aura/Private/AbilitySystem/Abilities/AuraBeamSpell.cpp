// Copyright 


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"

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
            OwnerPlayerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
        }
    }
}
