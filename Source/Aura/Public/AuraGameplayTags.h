// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
USTRUCT()
struct FAuraGameplayTags
{
public:
    static const FAuraGameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeGameplayTags();

private:
    static FAuraGameplayTags GameplayTags;
};