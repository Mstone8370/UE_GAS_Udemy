// Copyright 


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
 	PrimaryActorTick.bCanEverTick = false;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("SceneRootComponent");
	RootComponent = SceneRootComponent;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleComponent");
	MagicCircleDecal->SetupAttachment(RootComponent);
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagicCircle::SetMaterial(UMaterialInterface* InMaterial)
{
	if (MagicCircleDecal)
	{
		MagicCircleDecal->SetMaterial(0, InMaterial);
	}
}
