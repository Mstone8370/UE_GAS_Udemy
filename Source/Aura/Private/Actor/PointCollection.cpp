// Copyright 


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
    PrimaryActorTick.bCanEverTick = false;

    PointRoot = CreateDefaultSubobject<USceneComponent>(FName("PointRoot"));
    SetRootComponent(PointRoot);
    
    Point_1 = CreateDefaultSubobject<USceneComponent>(FName("Point_1"));
    Point_1->SetupAttachment(PointRoot);

    Point_2 = CreateDefaultSubobject<USceneComponent>(FName("Point_2"));
    Point_2->SetupAttachment(PointRoot);

    Point_3 = CreateDefaultSubobject<USceneComponent>(FName("Point_3"));
    Point_3->SetupAttachment(PointRoot);

    Point_4 = CreateDefaultSubobject<USceneComponent>(FName("Point_4"));
    Point_4->SetupAttachment(PointRoot);

    Point_5 = CreateDefaultSubobject<USceneComponent>(FName("Point_5"));
    Point_5->SetupAttachment(PointRoot);

    Point_6 = CreateDefaultSubobject<USceneComponent>(FName("Point_6"));
    Point_6->SetupAttachment(PointRoot);
}

void APointCollection::BeginPlay()
{
    Super::BeginPlay();

    Points.Add(PointRoot);
    Points.Add(Point_1);
    Points.Add(Point_2);
    Points.Add(Point_3);
    Points.Add(Point_4);
    Points.Add(Point_5);
    Points.Add(Point_6);
}

const TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 Level, float YawOverride)
{
    TArray<USceneComponent*> PointsCopy;
    bool bIsFirstPoint = true;
    int32 MaxPointsNum = Points.Num();
    if (Level == 1)
    {
        MaxPointsNum = 1;
    }
    else if (Level == 2)
    {
        MaxPointsNum = 4;
    }

    // 범위 내의 액터를 모두 찾고 아래의 LineTrace에서 무시하기 위함
    TArray<AActor*> IgnoreActors;
    UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

    for (USceneComponent* Pt : Points)
    {
        if (PointsCopy.Num() >= MaxPointsNum)
        {
            break;
        }

        if (!bIsFirstPoint)
        {
            FVector ToPoint = Pt->GetComponentLocation() - Points[0]->GetComponentLocation();
            ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
            Pt->SetWorldLocation(Points[0]->GetComponentLocation() + ToPoint);
        }
        else
        {
            bIsFirstPoint = false;
        }

        const FVector TraceStart = FVector(Pt->GetComponentLocation()) + FVector(0.f, 0.f, 500.f);
        const FVector TraceEnd = FVector(Pt->GetComponentLocation()) - FVector(0.f, 0.f, 500.f);

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActors(IgnoreActors);
        GetWorld()->LineTraceSingleByProfile(HitResult, TraceStart, TraceEnd, FName("BlockAll"), QueryParams);

        const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
        Pt->SetWorldLocation(AdjustedLocation);
        Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

        PointsCopy.Add(Pt);
    }

    return PointsCopy;
}

