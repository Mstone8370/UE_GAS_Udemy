// Copyright 


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    if (Ability->IsLocallyControlled())
    {
        // 클라이언트
        SendMouseCursorData();
    }
    else
    {
        // 서버
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
        AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
            this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback
        );
        // TargetData를 이미 받았으면 TargetSetDelegate를 broadcast하고 true 리턴.
        const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
        if (!bCalledDelegate)
        {
            // TargetData를 아직 받지 않았으면 기다림.
            // AbilitySystemComponent의 AbilityTargetDataMap에 넣어서 기억해둠.
            SetWaitingOnRemotePlayerData();
        }
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
        PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);
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

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
    FGameplayTag ActivationTag)
{
    // TargetData를 받기 위해 기다린 경우에는 AbilitySystemComponent의 AbilityTargetDataMap에 넣어서 기억해둔 데이터를 지움.
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}
