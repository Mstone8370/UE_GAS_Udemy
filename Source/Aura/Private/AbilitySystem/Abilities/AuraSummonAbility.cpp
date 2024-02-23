// Copyright 


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
    if (NumMinions <= 0)
    {
        return TArray<FVector>();
    }

    const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
    const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
    const float DeltaSpread = SpawnSpread / NumMinions;

    const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2, FVector::UpVector);
    const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2, FVector::UpVector);

    TArray<FVector> SpawnLocations;
    for (int32 i = 0; i < NumMinions; i++)
    {
        const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
        const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
        SpawnLocations.Add(ChosenSpawnLocation);

        DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f);

        UKismetSystemLibrary::DrawDebugArrow(
            GetAvatarActorFromActorInfo(),
            Location,
            Location + Direction * MaxSpawnDistance,
            4.f,
            FLinearColor::Green,
            3.f
        );
    }

    return TArray<FVector>();
}
