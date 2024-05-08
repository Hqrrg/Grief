// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Enums/Direction.h"
#include "Interfaces/PlatformPlayer.h"


// Sets default values
ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetBoxExtent(FVector(35.0f, 35.0f, 90.0f));
	SpawnBox->ShapeColor = FColor::Emerald;
	SetRootComponent(SpawnBox);

	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	DirectionArrow->SetArrowColor(FColor::Yellow);
	DirectionArrow->SetupAttachment(SpawnBox);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(70.0f, 70.0f, 90.0f));
	CollisionBox->ShapeColor = FColor::Green;
	CollisionBox->SetupAttachment(SpawnBox);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::BeginOverlap);
}

FVector ACheckpoint::GetSpawnLocation() const
{
	return SpawnBox->GetComponentLocation();
}

EDirection ACheckpoint::GetSpawnDirection() const
{
	FVector DirectionVector = DirectionArrow->GetForwardVector();
	EDirection Direction = EDirection::Right;

	float DirectionY = DirectionVector.Y;

	if (DirectionY > 0.0f)
	{
		Direction = EDirection::Right;
	}
	else if (DirectionY < 0.0f)
	{
		Direction = EDirection::Left;
	}

	return Direction;
}

void ACheckpoint::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	IPlatformPlayer* Player = Cast<IPlatformPlayer>(OtherActor);

	if (!Player) return;

	ACheckpoint* CurrentCheckpoint = Player->GetCheckpoint();
	int8 CurrentIndex = -1;

	if (CurrentCheckpoint) CurrentIndex = CurrentCheckpoint->GetIndex();
	if (CurrentIndex > Index) return;
	
	Player->SetCheckpoint(this);
}
