// Copyright 


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
    check(GEngine)
    UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
    return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

    FAuraGameplayTags::InitializeNativeGameplayTags();

    StaticInitTagsToCaptureDefMap(); // ExecCalc_Damage
}
