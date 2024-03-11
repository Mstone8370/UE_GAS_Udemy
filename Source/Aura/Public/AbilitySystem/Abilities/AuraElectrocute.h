// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "AuraElectrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetLextLevelDescription(int32 Level) override;

};
