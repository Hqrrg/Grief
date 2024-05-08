// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnTrigger.h"

#include "Components/BoxComponent.h"
#include "Interfaces/PlatformPlayer.h"


// Sets default values
AEnemySpawnTrigger::AEnemySpawnTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->ShapeColor = FColor::Yellow;
	SetRootComponent(TriggerBox);
}

// Called when the game starts or when spawned
void AEnemySpawnTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnTrigger::BeginOverlap);
}

void AEnemySpawnTrigger::BroadcastEnemySpawnTriggerOverlapped()
{
	OnEnemySpawnTriggerOverlapped.Broadcast();
	Triggered = true;
}

void AEnemySpawnTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Triggered) return;
	
	if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(OtherActor))
	{
		BroadcastEnemySpawnTriggerOverlapped();
	}
}

void AEnemySpawnTrigger::ResetPlatformActor()
{
	Triggered = false;
}
