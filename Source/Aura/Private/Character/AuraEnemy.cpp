// Copyright 


#include "Character/AuraEnemy.h"

#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
    if (IsValid(GetMesh()))
    {
        GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    }
    if (IsValid(Weapon))
    {
        Weapon->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    }
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
