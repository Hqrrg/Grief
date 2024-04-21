// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraBoundingBox.h"

#include "PlayerCharacter.h"
#include "PlatformCameraComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
ACameraBoundingBox::ACameraBoundingBox()
{
	PrimaryActorTick.bCanEverTick = false;

	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	SetRootComponent(BoundingBox);
	
	BoundingBox->ShapeColor = FColor::Turquoise;
}

// Called when the game starts or when spawned
void ACameraBoundingBox::BeginPlay()
{
	Super::BeginPlay();

	BoundingBox->OnComponentBeginOverlap.AddDynamic(this, &ACameraBoundingBox::BeginOverlap);
	BoundingBox->OnComponentEndOverlap.AddDynamic(this, &ACameraBoundingBox::EndOverlap);
}

void ACameraBoundingBox::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const APlayerCharacter* GriefCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		UPlatformCameraComponent* PlatformCameraComponent = GriefCharacter->GetPlatformCameraComponent();
		PlatformCameraComponent->AddCameraBoundingBox(this);
	}
}

void ACameraBoundingBox::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const APlayerCharacter* GriefCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		UPlatformCameraComponent* PlatformCameraComponent = GriefCharacter->GetPlatformCameraComponent();
		PlatformCameraComponent->RemoveCameraBoundingBox(this);
	}
}

float ACameraBoundingBox::GetBound(EDirection Direction)
{
	float Bound = 0.0f;

	const FVector ActorLocation = GetActorLocation();
	const FVector BoxExtent = BoundingBox->GetScaledBoxExtent();

	switch (Direction)
	{
	case EDirection::Up:
		Bound = ActorLocation.Z + BoxExtent.Z;
		break;
	case EDirection::Down:
		Bound = ActorLocation.Z - BoxExtent.Z;
		break;
	case EDirection::Left:
		Bound = ActorLocation.Y - BoxExtent.Y;
		break;
	case EDirection::Right:
		Bound = ActorLocation.Y + BoxExtent.Y;
		break;
	default:
		break;
	}
	
	return Bound;
}

bool ACameraBoundingBox::IsBoundActive(EDirection Direction)
{
	bool BoundActive = false;

	switch (Direction)
	{
	case EDirection::Up:
		BoundActive = Up;
		break;
	case EDirection::Down:
		BoundActive = Down;
		break;
	case EDirection::Left:
		BoundActive = Left;
		break;
	case EDirection::Right:
		BoundActive = Right;
		break;
	default:
		break;
	}
	
	return BoundActive;
}

