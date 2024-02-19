// Copyright 


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *OwnerComp.GetAIOwner()->GetName());
    GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Green, *OwnerComp.GetOwner()->GetName());

    OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName);

    const APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

    const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

    
}
