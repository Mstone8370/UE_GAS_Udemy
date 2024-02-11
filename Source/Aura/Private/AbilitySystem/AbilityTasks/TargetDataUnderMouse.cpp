// Copyright 


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "Character/AuraCharacter.h"
#include "Player/AuraPlayerController.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    // MyObj->Ability = OwningAbility;
    return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    if (!Ability)
    {
        return;
    }
    
    if (const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
    {
        FHitResult CursorHit;
        if (PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit))
        {
            const FVector Location = CursorHit.ImpactPoint;
            ValidData.Broadcast(Location);
        }
    }
}
