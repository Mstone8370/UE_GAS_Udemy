// Copyright 


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
    NodeName = FString("FindNearestPlayer");
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    const APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

    const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

    float ClosestDistance = TNumericLimits<float>::Max();
    AActor* ClosestActor = nullptr;
    for (AActor* Actor : ActorsWithTag)
    {
        if (IsValid(Actor) && IsValid(OwningPawn))
        {
            const float Distance = OwningPawn->GetDistanceTo(Actor);
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestActor = Actor;
            }
        }
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("TargetToFollow"), ClosestActor);
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FName("DistanceToTarget"), ClosestDistance);
}
