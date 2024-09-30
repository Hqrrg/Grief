// Copyright Epic Games, Inc. All Rights Reserved.


#include "GriefGameModeBase.h"

#include "CameraBoundingBox.h"
#include "Checkpoint.h"
#include "EnemySpawnTrigger.h"
#include "EngineUtils.h"
#include "PlayerPawn.h"
#include "Interfaces\PlatformActorInterface.h"


APlayerController* AGriefGameModeBase::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	PlayerControllerRef = Super::SpawnPlayerController(InRemoteRole, Options);
	return PlayerControllerRef;
}


APawn* AGriefGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	PlayerRef = SpawnPlayer();
	return PlayerRef;
}

void AGriefGameModeBase::RespawnPlayer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Iterate through all actors
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		IPlatformActorInterface* PlatformActor = Cast<IPlatformActorInterface>(*It);
		if (!PlatformActor) continue;
		// Reset platform actors	
		PlatformActor->ResetPlatformActor();
	}
	PlayerRef = SpawnPlayer();

	TSet<AActor*> OverlappingActors;
	PlayerRef->GetOverlappingActors(OverlappingActors);

	// Call BeginOverlap logic for camera bounding boxes and enemy spawn triggers if spawned inside
	for (AActor* OverlappingActor : OverlappingActors)
	{
		ACameraBoundingBox* CameraBoundingBox = Cast<ACameraBoundingBox>(OverlappingActor);
		if (CameraBoundingBox)
		{
			CameraBoundingBox->AddCameraBoundingBox(PlayerRef);
			continue;
		}
		AEnemySpawnTrigger* EnemySpawnTrigger = Cast<AEnemySpawnTrigger>(OverlappingActor);
		if (EnemySpawnTrigger)
		{
			EnemySpawnTrigger->BroadcastEnemySpawnTriggerOverlapped();
		}
	}
}

APlayerPawn* AGriefGameModeBase::SpawnPlayer()
{
	ACheckpoint* DefaultCheckpoint = FindDefaultCheckpoint();
	if (!DefaultCheckpoint) return nullptr;

	// Defaults
	FVector DefaultCheckpointLocation = DefaultCheckpoint->GetSpawnLocation();
	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, DefaultCheckpointLocation, FVector::OneVector);
	EDirection SpawnDirection = DefaultCheckpoint->GetSpawnDirection();
	
	if (!PlayerRef) // Spawning for first time
	{
		UWorld* World = GetWorld();
		if (!World) return nullptr;

		// Spawn
		APlayerPawn* PlayerPawn = World->SpawnActorDeferred<APlayerPawn>(DefaultPawnClass->GetAuthoritativeClass(), SpawnTransform);
		if (!PlayerPawn) return nullptr;

		// Set checkpoint to default
		PlayerPawn->SetCheckpoint(DefaultCheckpoint);
		
		PlayerPawn->FinishSpawning(SpawnTransform);
		PlayerPawn->SetMovementDirection(SpawnDirection);
		// Broadcast notify dispatcher
		PlayerPawn->BroadcastPlayerSpawned(false);
		
		return PlayerPawn;
	}

	// Update spawn information to checkpoint information
	if (ACheckpoint* Checkpoint = PlayerRef->GetCheckpoint())
	{
		FVector CheckpointLocation = Checkpoint->GetSpawnLocation();
		SpawnTransform.SetLocation(CheckpointLocation);
		SpawnDirection = Checkpoint->GetSpawnDirection();
	}

	// Respawn | teleport & update movement direction
	FVector SpawnLocation = SpawnTransform.GetLocation();
	FRotator SpawnRotation = SpawnTransform.GetRotation().Rotator();
	
	PlayerRef->TeleportTo(SpawnLocation, SpawnRotation);
	PlayerRef->SetMovementDirection(SpawnDirection);
	// Broadcast notify dispatcher
	PlayerRef->BroadcastPlayerSpawned(true);

	return PlayerRef;
}

ACheckpoint* AGriefGameModeBase::FindDefaultCheckpoint()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// Iterate through all checkpoint actors in the world
	for (TActorIterator<ACheckpoint> It(World); It; ++It)
	{
		ACheckpoint* Checkpoint = *It;

		// Checkpoint valid & marked default: return
		if (Checkpoint && Checkpoint->IsDefault())
		{
			return Checkpoint;
		}
	}
	return nullptr;
}
