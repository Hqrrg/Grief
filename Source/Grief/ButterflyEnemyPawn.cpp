// Fill out your copyright notice in the Description page of Project Settings.


#include "ButterflyEnemyPawn.h"

#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"


// Sets default values
AButterflyEnemyPawn::AButterflyEnemyPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PathCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PathCollisionComponent"));
	PathCollisionComponent->ShapeColor = FColor::Green;
	SetRootComponent(PathCollisionComponent);
	
	MovementPath = CreateDefaultSubobject<USplineComponent>(TEXT("MovementPath"));
	MovementPath->SetClosedLoop(true);
	MovementPath->SetupAttachment(PathCollisionComponent);

	CollisionComponent->SetBoxExtent(FVector(35.0f, 35.0f, 35.0f));
	CollisionComponent->SetupAttachment(MovementPath);

	GetPlatformMovementComponent()->SetUpdatedComponent(PathCollisionComponent);
	GetPlatformMovementComponent()->SetFlying();
}

// Called when the game starts or when spawned
void AButterflyEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AButterflyEnemyPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FVector Offset = FVector(0.0f, MovementPathRadius * 1.5, 0.0f);
	
	PathCollisionComponent->SetBoxExtent(FVector(35.0f, MovementPathRadius, MovementPathRadius) + Offset + FVector(0.0f, 50.0f, 50.0f));
	
	MovementPath->ClearSplinePoints();

	const FVector PointLocationZ = FVector(0.0f, 0.0f, MovementPathRadius);
	const FVector PointLocationY = FVector(0.0f, MovementPathRadius, 0.0f);
	
	MovementPath->AddSplineLocalPoint(-PointLocationZ - Offset);
	MovementPath->AddSplineLocalPoint(-PointLocationY - Offset);
	MovementPath->AddSplineLocalPoint(PointLocationZ - Offset);
	MovementPath->AddSplineLocalPoint(-PointLocationZ + Offset);
	MovementPath->AddSplineLocalPoint(PointLocationY + Offset);
	MovementPath->AddSplineLocalPoint(PointLocationZ + Offset);

	float TangentFactor = 4 * (sqrt(2) - 1) / 3 + 1;

	FVector ZTangent = FVector(0.0f, 0.0f, MovementPathRadius * TangentFactor);
	FVector YTangent = FVector(0.0f, MovementPathRadius * TangentFactor, 0.0f);

	FVector IntersectionYTangent = FVector(0.0f, MovementPathRadius * (TangentFactor*2), 0.0f);

	MovementPath->SetTangentAtSplinePoint(1, ZTangent, ESplineCoordinateSpace::Local);
	MovementPath->SetTangentAtSplinePoint(4, ZTangent, ESplineCoordinateSpace::Local);
	
	MovementPath->SetTangentsAtSplinePoint(0, -IntersectionYTangent, -YTangent, ESplineCoordinateSpace::Local);
	MovementPath->SetTangentsAtSplinePoint(2, YTangent, IntersectionYTangent, ESplineCoordinateSpace::Local);
	MovementPath->SetTangentsAtSplinePoint(3, IntersectionYTangent, YTangent, ESplineCoordinateSpace::Local);
	MovementPath->SetTangentsAtSplinePoint(5, -YTangent, -IntersectionYTangent, ESplineCoordinateSpace::Local);
}

float Speed = 3.0f;
float Alpha = 0.0f;

// Called every frame
void AButterflyEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Alpha+=FMath::Clamp(1.0f / Speed * DeltaTime, 0.0f, 1.0f);
	
	if (Alpha >= 1.0f) Alpha = 0.0f;

	float DistanceAlongSpline = FMath::Lerp(0, MovementPath->GetSplineLength(), Alpha);
	FVector TargetLocation = MovementPath->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	FHitResult* SweepResult = new FHitResult();

	GetCollisionComponent()->SetWorldLocation(TargetLocation);
	GetCollisionComponent()->UpdateOverlaps(nullptr, true, nullptr);
}
