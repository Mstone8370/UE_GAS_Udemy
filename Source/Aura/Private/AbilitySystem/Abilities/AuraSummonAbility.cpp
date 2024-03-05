// Copyright 


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
    if (NumMinions <= 0)
    {
        return TArray<FVector>();
    }

    const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
    const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

    TArray<FVector> Directions;
    UAuraAbilitySystemLibrary::GetEvenlyRotatedVectors(
        Forward, SpawnSpread, NumMinions, FVector::UpVector, Directions
    );

    TArray<FVector> SpawnLocations;
    for (const FVector& Direction : Directions)
    {
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
