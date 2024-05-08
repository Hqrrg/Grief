// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleProjectileMovement.h"

#include "Components/ShapeComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
USimpleProjectileMovement::USimpleProjectileMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USimpleProjectileMovement::BeginPlay()
{
	Super::BeginPlay();
	
	UpdatedComponent = GetOwner()->GetRootComponent();
	CollisionComponent = Cast<UShapeComponent>(UpdatedComponent);

	if (!CollisionComponent) UninitializeComponent();
}

void USimpleProjectileMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	HandleMovement(DeltaTime);
}

void USimpleProjectileMovement::HandleMovement(float DeltaTime)
{
	if (!ShouldMove) return;
	if (MovementVector == FVector::ZeroVector) return;

	// Manipulate Movement Vector
	FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
	FVector TargetLocation = CurrentLocation + MovementVector * Speed * DeltaTime;
	FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetLocation - CurrentLocation).Rotator(); TargetRotation.Roll = 0.0f;
	
	if (CheckCollided(CollisionHitResult, TargetLocation))
	{
		TargetLocation = CollisionHitResult.Location;
		HandleProjectileHit(CollisionHitResult.GetActor());
	};
	
	FLatentActionInfo LatentInfo; LatentInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(
		UpdatedComponent,
		TargetLocation,
		TargetRotation,
		false, false,
		DeltaTime,
		false,
		EMoveComponentAction::Move,
		LatentInfo);
}

bool USimpleProjectileMovement::CheckCollided(FHitResult& HitResult, FVector TraceEnd)
{
	FCollisionQueryParams QueryParams; QueryParams.AddIgnoredActor(GetOwner());
	FCollisionShape CollisionShape = CollisionComponent->GetCollisionShape();
	FName ProfileName = CollisionComponent->GetCollisionProfileName();

	FVector TraceStart = CollisionComponent->GetComponentLocation();
	FQuat Quaternion = CollisionComponent->GetComponentQuat();
	
	bool IsBlocked = GetWorld()->SweepSingleByProfile(
		HitResult,
		TraceStart,
		TraceEnd,
		Quaternion,
		ProfileName,
		CollisionShape,
		QueryParams);

	if (IsBlocked) return true;
	
	return false;
}

void USimpleProjectileMovement::HandleProjectileHit(AActor* HitActor)
{
	OnProjectileHit.Broadcast(HitActor);
	ShouldMove = false;
}

void USimpleProjectileMovement::SetMovementVector(FVector InMovementVector)
{
	float X = FMath::Clamp(InMovementVector.X, -1.0f, 1.0f);
	float Y = FMath::Clamp(InMovementVector.Y, -1.0f, 1.0f);
	float Z = FMath::Clamp(InMovementVector.Z, -1.0f, 1.0f);

	MovementVector = FVector(X, Y, Z);

	ShouldMove = true;
}

