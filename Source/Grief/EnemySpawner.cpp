// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "EnemyPawn.h"
#include "EnemySpawnParamaters.h"
#include "EnemySpawnTrigger.h"
#include "Components/BoxComponent.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	EnemySpawnParamaters = NewObject<UEnemySpawnParamaters>();

	PreviewBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PreviewBox"));
	PreviewBox->SetBoxExtent(FVector(35.0f, 35.0f, 90.0f));
	PreviewBox->ShapeColor = FColor::Red;
	SetRootComponent(PreviewBox);
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnOnBeginPlay) Spawn();
	
	if (SpawnTrigger)
	{
		SpawnTrigger->OnEnemySpawnTriggerOverlapped.AddDynamic(this, &AEnemySpawner::Spawn);
	}
}

void AEnemySpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEnemySpawner::Spawn()
{
	UWorld* World = GetWorld();

	if (!World) return;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString("World"));
	
	SpawnTransform = GetActorTransform();
	EnemyPawn = World->SpawnActorDeferred<AEnemyPawn>(EnemyClass->GetAuthoritativeClass(), SpawnTransform);

	if (!EnemyPawn) return;

	EnemyPawn->SetSpawnParamaters(EnemySpawnParamaters);
	EnemyPawn->FinishSpawning(SpawnTransform);

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString("Spawned"));
}

void AEnemySpawner::ResetPlatformActor()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	
	if (!SpawnOnBeginPlay) return;
	
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnTimerDelegate;

	RespawnTimerDelegate.BindUFunction(this, FName("Spawn"));

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnTimerDelegate, 1.0f, false, 1.0f);
}

