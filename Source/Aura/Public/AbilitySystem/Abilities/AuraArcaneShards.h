// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetLextLevelDescription(int32 Level) override;
	
};
