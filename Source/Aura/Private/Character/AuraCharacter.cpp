// Copyright 


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Init ability actor info for the Server
    InitAbilityActorInfo();
    AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // Init ability actor info for the Client
    InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
    const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::InitAbilityActorInfo()
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
    AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
    AttributeSet = AuraPlayerState->GetAttributeSet();

    // 멀티플레이어인 경우 다른 플레이어의 Controller는 캐스팅에 실패함.
    if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
    {
        // on Server and Client

        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
        {
            // on Client

            AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
        }
    }

    if (HasAuthority())
    {
        // 모든 어트리뷰트는 레플리케이트 되므로 아래 함수는 서버에서만 호출해도 됨.
        InitializeDefaultAttributes();
    }
}
