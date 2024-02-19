// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AURA_API UBTService_FindNearestPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
