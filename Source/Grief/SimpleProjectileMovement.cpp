// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleProjectileMovement.h"

#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
USimpleProjectileMovement::USimpleProjectileMovement()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USimpleProjectileMovement::BeginPlay()
{
	Super::BeginPlay();
	UpdatedComponent = GetOwner()->GetRootComponent();
}

void USimpleProjectileMovement::UpdateDestination(FVector InDestination)
{
	Destination = InDestination;
	
	FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
	FVector Distance = CurrentLocation - Destination;

	FVector MovementVector = (Destination - CurrentLocation).GetSafeNormal();
	FVector OvershootDestination = CurrentLocation + MovementVector * FVector(0.0f, Distance.Y, Distance.Z).Length() * 2.0f;
	FVector OvershootDistance = CurrentLocation - OvershootDestination;
	float EstimatedDuration = FVector(0.0f, OvershootDistance.Y, OvershootDistance.Z).Length() / Speed;
	
	FLatentActionInfo LatentInfo; LatentInfo.CallbackTarget = this;
	
	UKismetSystemLibrary::MoveComponentTo(
		UpdatedComponent,
		OvershootDestination,
		UpdatedComponent->GetRelativeRotation(),
		false, false,
		EstimatedDuration,
		false,
		EMoveComponentAction::Move,
		LatentInfo);
}

