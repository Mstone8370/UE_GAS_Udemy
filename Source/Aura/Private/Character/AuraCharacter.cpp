// Copyright 


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

AAuraCharacter::AAuraCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->bDoCollisionTest = false;

    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation = false;

    LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
    LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
    LevelUpNiagaraComponent->bAutoActivate = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    CharacterClass = ECharacterClass::Elementalist;

    BaseWalkSpeed = 600.f;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Init ability actor info for the Server
    if (!OnASCRegistered.IsBoundToObject(this))
    {
        OnASCRegistered.AddUObject(this, &AAuraCharacter::OnAbilitySystemComponentRegistered);
    }
    InitAbilityActorInfo();
    AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // Init ability actor info for the Client
    if (!OnASCRegistered.IsBoundToObject(this))
    {
        OnASCRegistered.AddUObject(this, &AAuraCharacter::OnAbilitySystemComponentRegistered);
    }
    InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
    const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->GetPlayerLevel();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetXP_Implementation() const
{
    const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->GetXP();
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
    const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
    const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AuraPlayerState->AddToLevel(InPlayerLevel);

    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
    {
        AuraASC->UpdateAbilityStatus(ICombatInterface::Execute_GetPlayerLevel(this));
    }
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::LevelUp_Implementation()
{
    Multicast_LevelUpParticles();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
    if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
    {
        AuraPlayerController->ShowMagicCircle();
    }
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
    if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
    {
        AuraPlayerController->HideMagicCircle();
    }
}

void AAuraCharacter::Multicast_LevelUpParticles_Implementation()
{
    if (IsValid(LevelUpNiagaraComponent))
    {
        const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
        const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
        const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
        LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);

        LevelUpNiagaraComponent->Activate(true);
    }
}

void AAuraCharacter::OnRep_IsStunned(bool bOldIsStunned)
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    FGameplayTagContainer BlockedTags;
    BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
    BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
    BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
    BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);

    if (bIsStunned)
    {
        AbilitySystemComponent->AddLooseGameplayTags(BlockedTags);
        if (StunDebuffComponent)
        {
            StunDebuffComponent->Activate();
        }
    }
    else
    {
        AbilitySystemComponent->RemoveLooseGameplayTags(BlockedTags);
        if (StunDebuffComponent)
        {
            StunDebuffComponent->Deactivate();
        }
    }
}

void AAuraCharacter::OnRep_IsBurned(bool bOldIsStunned)
{
    if (bIsBurned)
    {
        if (BurnDebuffComponent)
        {
            BurnDebuffComponent->Activate();
        }
    }
    else
    {
        if (BurnDebuffComponent)
        {
            BurnDebuffComponent->Deactivate();
        }
    }
}

void AAuraCharacter::InitAbilityActorInfo()
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState);
    AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
    AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
    AttributeSet = AuraPlayerState->GetAttributeSet();

    OnASCRegistered.Broadcast(AbilitySystemComponent);
    // AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacterBase::StunTagChanged);

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
