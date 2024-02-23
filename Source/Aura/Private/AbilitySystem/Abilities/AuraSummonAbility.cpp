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
        FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

        FHitResult Hit;
        const FVector Start = ChosenSpawnLocation + FVector(0.f, 0.f, 400.f);
        const FVector End = ChosenSpawnLocation - FVector(0.f, 0.f, 400.f);
        GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

        if (Hit.bBlockingHit)
        {
            ChosenSpawnLocation = Hit.Location;
        }
        SpawnLocations.Add(ChosenSpawnLocation);
    }

    return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
    return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
