// Copyright 


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    if (Ability->IsLocallyControlled())
    {
        SendMouseCursorData();
    }
    else
    {
        // TODO: We are on the server, to listen for target data;
    }
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
    // 이 scope에 있는 것들은 prediction되어야한다는 의미
    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
    
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();

    if (const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
    {
        FHitResult CursorHit;
        PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
        Data->HitResult = CursorHit;
    }

    FGameplayAbilityTargetDataHandle DataHandle;
    DataHandle.Add(Data);
    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),
        GetActivationPredictionKey(),
        DataHandle,
        FGameplayTag(),
        AbilitySystemComponent->ScopedPredictionKey
    );

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}
