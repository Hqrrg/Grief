// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSensingComponent.h"

#include "PlayerPawn.h"
#include "Components/BoxComponent.h"


// Sets default values for this component's properties
UPlayerSensingComponent::UPlayerSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bPlayerDetected = false;
	bPlayerSensed = false;
}


// Called when the game starts
void UPlayerSensingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Enabled) return;
	
	PlayerController = GetOwner()->GetWorld()->GetFirstPlayerController();
	PlayerPawn = Cast<APlayerPawn>(PlayerController->GetPawn());
	
	StartTimer(TimerInterval);
}

void UPlayerSensingComponent::StartTimer(float Interval)
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("Update"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Interval, true, false);
}

void UPlayerSensingComponent::Update()
{
	if (PlayerPawn)
	{
		FVector SensingPawnLocation = GetOwner()->GetActorLocation();
		FVector PawnLocation = PlayerPawn->GetActorLocation();
		FVector SensingToPawn = SensingPawnLocation - PawnLocation;

		float DistanceToPawn = SensingToPawn.Length();

		// Outside search radius
		if (DistanceToPawn > GetSearchRadius())
		{
			// Was sensed - return to slower check intervals
			if (bPlayerSensed)
			{
				bPlayerSensed = false;
				TimerInterval = 0.5f;
				StartTimer(TimerInterval);
			}
			// Was detected- player escaped
			if (bPlayerDetected)
			{
				bPlayerDetected = false;
				BroadcastPlayerEscaped();
			}
			return;
		}
		// Outside detection radius, within search radius
		if (DistanceToPawn > GetDetectionRadius())
		{
			// Wasn't sensed - player sensed not detected
			if (!bPlayerSensed)
			{
				bPlayerSensed = true;
				TimerInterval = 0.1f;
				StartTimer(TimerInterval);
				BroadcastPlayerSensed(ESensingRange::Search);
			}
			// Player left detection radius
			if (bPlayerDetected && LastSensingRange == ESensingRange::Detection)
			{
				BroadcastPlayerSensed(ESensingRange::Search);
			}
			return;
		}
		if (bPlayerDetected) return;
		// Detected
		bPlayerDetected = true;
		BroadcastPlayerSensed(ESensingRange::Detection);
	}
}

void UPlayerSensingComponent::BroadcastPlayerSensed(ESensingRange SensingRange)
{
	OnPlayerSensed.Broadcast(PlayerPawn, bPlayerDetected, SensingRange, HasLineOfSight());
	LastSensingRange = SensingRange;
}

void UPlayerSensingComponent::BroadcastPlayerEscaped()
{
	OnPlayerEscaped.Broadcast(PlayerPawn);
	LastSensingRange = ESensingRange::None;
}

bool UPlayerSensingComponent::HasLineOfSight()
{
	bool bHasLineOfSight = false;

	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams QueryParams; QueryParams.AddIgnoredActor(GetOwner());
	FName CollisionProfileName = PlayerPawn->GetCollisionComponent()->GetCollisionProfileName();

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector SensingPawnLocation = GetOwner()->GetActorLocation();

	bHasLineOfSight = GetWorld()->LineTraceSingleByProfile(
		*HitResult,
		SensingPawnLocation,
		PlayerLocation,
		CollisionProfileName,
		QueryParams);
	
	return bHasLineOfSight;
}

#if WITH_EDITOR
void UPlayerSensingComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	DetectionRadius = FMath::Clamp(DetectionRadius, 128.0f, SearchRadius);
	SearchRadius = FMath::Clamp(SearchRadius, DetectionRadius + 128.0f, 10000.0f);
}
#endif
