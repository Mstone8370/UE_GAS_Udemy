// Copyright 


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
    : Level(1)
{
    if (IsValid(GetMesh()))
    {
        GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    }
    if (IsValid(Weapon))
    {
        Weapon->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    }
    
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->SetIsReplicated(true);
        AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
    }
    
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::BeginPlay()
{
    Super::BeginPlay();
    
}

void AAuraEnemy::InitAbilityActorInfo()
{
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}

void AAuraEnemy::HighlightActor()
{
    if (IsValid(GetMesh()))
    {
        GetMesh()->SetRenderCustomDepth(true);
        GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    }
    if (IsValid(Weapon))
    {
        Weapon->SetRenderCustomDepth(true);
        Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    }
}

void AAuraEnemy::UnHighlightActor()
{
    if (IsValid(GetMesh()))
    {
        GetMesh()->SetRenderCustomDepth(false);
    }
    if (IsValid(Weapon))
    {
        Weapon->SetRenderCustomDepth(false);
    }
}

int32 AAuraEnemy::GetPlayerLevel()
{
    return Level;
}
