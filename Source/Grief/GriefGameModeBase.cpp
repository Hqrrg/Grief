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
	
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		IPlatformActorInterface* PlatformActor = Cast<IPlatformActorInterface>(*It);

		if (!PlatformActor) continue;
		
		PlatformActor->ResetPlatformActor();
	}

	PlayerRef = SpawnPlayer();

	TSet<AActor*> OverlappingActors;
	PlayerRef->GetOverlappingActors(OverlappingActors);

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
	
	FVector DefaultCheckpointLocation = DefaultCheckpoint->GetSpawnLocation();
	
	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, DefaultCheckpointLocation, FVector::OneVector);
	EDirection SpawnDirection = DefaultCheckpoint->GetSpawnDirection();
	
	if (!PlayerRef)
	{
		UWorld* World = GetWorld();

		if (!World) return nullptr;

		APlayerPawn* PlayerPawn = World->SpawnActorDeferred<APlayerPawn>(DefaultPawnClass->GetAuthoritativeClass(), SpawnTransform);

		if (!PlayerPawn) return nullptr;
		
		if (ACheckpoint* Checkpoint = PlayerPawn->GetCheckpoint())
		{
			FVector CheckpointLocation = Checkpoint->GetSpawnLocation();
			SpawnTransform.SetLocation(CheckpointLocation);
			SpawnDirection = Checkpoint->GetSpawnDirection();
		}
		else
		{
			PlayerPawn->SetCheckpoint(DefaultCheckpoint);
		}
		
		PlayerPawn->FinishSpawning(SpawnTransform);
		PlayerPawn->SetMovementDirection(SpawnDirection);

		PlayerPawn->BroadcastPlayerSpawned(false);
		
		return PlayerPawn;
	}

	if (ACheckpoint* Checkpoint = PlayerRef->GetCheckpoint())
	{
		FVector CheckpointLocation = Checkpoint->GetSpawnLocation();
		SpawnTransform.SetLocation(CheckpointLocation);
		SpawnDirection = Checkpoint->GetSpawnDirection();
	}

	FVector SpawnLocation = SpawnTransform.GetLocation();
	FRotator SpawnRotation = SpawnTransform.GetRotation().Rotator();
	
	PlayerRef->TeleportTo(SpawnLocation, SpawnRotation);
	PlayerRef->SetMovementDirection(SpawnDirection);

	PlayerRef->BroadcastPlayerSpawned(true);

	return PlayerRef;
}

ACheckpoint* AGriefGameModeBase::FindDefaultCheckpoint()
{
	UWorld* World = GetWorld();

	if (!World) return nullptr;
	
	for (TActorIterator<ACheckpoint> It(World); It; ++It)
	{
		ACheckpoint* Checkpoint = *It;
		
		if (Checkpoint && Checkpoint->IsDefault())
		{
			return Checkpoint;
		}
	}
	
	return nullptr;
}
