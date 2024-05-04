// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementBoundingBox.h"

#include "Components/BoxComponent.h"


// Sets default values
AMovementBoundingBox::AMovementBoundingBox()
{
	// Disable tick for this actor to improve performance
	PrimaryActorTick.bCanEverTick = false;

	/* Create and attach box component */
	NavigationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("NavigationBox"));
	NavigationBox->SetupAttachment(RootComponent);
}

/* Called when this actor is spawned */
void AMovementBoundingBox::BeginPlay()
{
	Super::BeginPlay();

	/* Work out the locations of the two opposing corners of the box component */
	FVector Center = NavigationBox->Bounds.GetBox().GetCenter();
	FVector Size = NavigationBox->Bounds.GetBox().GetSize();
	FVector HalfSize2D = FVector(Size.X, Size.Y, Size.Z) / 2;
	FVector PointA = Center - HalfSize2D;
	FVector PointB = Center + HalfSize2D;

	/* Work out the minimum location */
	float MinX = FMath::Min(PointA.X, PointB.X);
	float MinY = FMath::Min(PointA.Y, PointB.Y);
	float MinZ = FMath::Min(PointA.Z, PointB.Z);
	MinLocation = FVector(MinX, MinY, MinZ);

	/* Work out the maximum location */
	float MaxX = FMath::Max(PointA.X, PointB.X);
	float MaxY = FMath::Max(PointA.Y, PointB.Y);
	float MaxZ = FMath::Max(PointA.Z, PointB.Z);
	MaxLocation = FVector(MaxX, MaxY, MaxZ);
}

/* Return whether a provided location falls within the navigation box */
bool AMovementBoundingBox::IsLocationWithinArea(FVector& Location)
{
	bool WithinX = MinLocation.X <= Location.X && MaxLocation.X >= Location.X;
	bool WithinY = MinLocation.Y <= Location.Y && MaxLocation.Y >= Location.Y;
	bool WithinZ = MinLocation.Z <= Location.Z && MaxLocation.Z >= Location.Z;

	return WithinX && WithinY && WithinZ;
}

