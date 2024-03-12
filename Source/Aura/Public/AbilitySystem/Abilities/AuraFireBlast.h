// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetLextLevelDescription(int32 Level) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireBlast")
	int32 NumFireBalls = 12;
};
