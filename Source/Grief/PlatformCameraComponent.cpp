// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformCameraComponent.h"

#include "CameraBoundingBox.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UPlatformCameraComponent::UPlatformCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlatformCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* OwnerActor = GetOwner())
	{
		Camera = Cast<UCameraComponent>(OwnerActor->GetComponentByClass(UCameraComponent::StaticClass()));

		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, false);
		Camera->DetachFromComponent(DetachmentTransformRules);

		SetupCamera();
	}
}


// Called every frame
void UPlatformCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector ActorLocation = GetOwner()->GetActorLocation();
	const FVector CameraLocation = Camera->GetComponentLocation();

	float YTargetBias = 0.0f;
	float ZTargetBias = 0.0f;
	float YInterpSpeed = 5.0f;
	float ZInterpSpeed = 10.0f;

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		EDirection MovementDirection = PlayerCharacter->GetMovementDirection();
		
		switch (MovementDirection)
		{
		case EDirection::Left:
			YTargetBias = -1.0f;
			break;
			
		case EDirection::Right:
			YTargetBias = 1.0f;
			break;
			
		default:
			break;
		}

		EPlatformerMovementMode PlatformerMovementMode = PlayerCharacter->GetPlatformerMovementMode();

		switch (PlatformerMovementMode)
		{
		case EPlatformerMovementMode::Jumping:
			ZTargetBias = -1.0f;
			break;
			
		case EPlatformerMovementMode::Falling:
			ZTargetBias = 1.0f;
			break;
			
		default:
			break;
		}
	}

	YTargetBias*=YawMovementBias;
	ZTargetBias*=PitchMovementBias;

	float TargetY = ActorLocation.Y+YTargetBias;
	float TargetZ = ActorLocation.Z+ZTargetBias;

	
	if (CameraBounds.RightActive && TargetY > CameraBounds.Right)
	{
		TargetY = CameraBounds.Right;
	}
	if (CameraBounds.LeftActive && TargetY < CameraBounds.Left)
	{
		TargetY = CameraBounds.Left;
	}

	if (CameraBounds.UpActive && TargetZ > CameraBounds.Up)
	{
		TargetZ = CameraBounds.Up;
	}
	if (CameraBounds.DownActive && TargetZ < CameraBounds.Down)
	{
		TargetZ = CameraBounds.Down;
	}

	if (FMath::IsNearlyEqual(GetOwner()->GetVelocity().Length(), 0.0f, 0.5f))
	{
		YInterpSpeed = 2.5f;
		ZInterpSpeed = 5.0f;
	}
		
	const float InterpTargetY = FMath::FInterpTo(CameraLocation.Y, TargetY, DeltaTime, YInterpSpeed);
	const float InterpTargetZ = FMath::FInterpTo(CameraLocation.Z, TargetZ, DeltaTime, ZInterpSpeed);

	const FVector TargetLocation = FVector(ActorLocation.X - DistanceToCharacter, InterpTargetY, InterpTargetZ);
	
	Camera->SetWorldLocation(TargetLocation);
}

void UPlatformCameraComponent::AddCameraBoundingBox(ACameraBoundingBox* CameraBoundingBox)
{
	CameraBoundingBoxes.AddUnique(CameraBoundingBox);
	UpdateCameraBounds();
}

void UPlatformCameraComponent::RemoveCameraBoundingBox(ACameraBoundingBox* CameraBoundingBox)
{
	CameraBoundingBoxes.Remove(CameraBoundingBox);
	UpdateCameraBounds();
}

void UPlatformCameraComponent::SetupCamera()
{
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	const FVector TargetLocation = FVector(ActorLocation.X - DistanceToCharacter, ActorLocation.Y, ActorLocation.Z);
	
	Camera->SetWorldLocation(TargetLocation);
}

void UPlatformCameraComponent::UpdateCameraBounds()
{
	int32 UpLayer = CameraBounds.UpLayer;
	int32 DownLayer = CameraBounds.DownLayer;
	int32 LeftLayer = CameraBounds.LeftLayer;
	int32 RightLayer = CameraBounds.RightLayer;

	float Up = CameraBounds.Up;
	float Down = CameraBounds.Down;
	float Left = CameraBounds.Left;
	float Right = CameraBounds.Right;
	
	for (uint8 Index = 0; Index < CameraBoundingBoxes.Num(); Index++)
	{
		if (ACameraBoundingBox* BoundingBox = CameraBoundingBoxes[Index])
		{
			const float BoundingBoxUp = BoundingBox->GetBound(EDirection::Up);
			const float BoundingBoxDown = BoundingBox->GetBound(EDirection::Down);
			const float BoundingBoxLeft = BoundingBox->GetBound(EDirection::Left);
			const float BoundingBoxRight = BoundingBox->GetBound(EDirection::Right);

			const int32 BoundingBoxLayer = BoundingBox->GetLayer();
			
			if (BoundingBox->IsBoundActive(EDirection::Up) && (Up < BoundingBoxUp || BoundingBoxLayer > UpLayer))
			{
				Up = BoundingBoxUp;
				UpLayer = BoundingBoxLayer;
			}
			
			if (BoundingBox->IsBoundActive(EDirection::Down) && (Down > BoundingBoxDown || BoundingBoxLayer > DownLayer))
			{
				Down = BoundingBoxDown;
				DownLayer = BoundingBoxLayer;
			}

			if (BoundingBox->IsBoundActive(EDirection::Left) && (Left > BoundingBoxLeft || BoundingBoxLayer > LeftLayer))
			{
				Left = BoundingBoxLeft;
				LeftLayer = BoundingBoxLayer;
			}

			if (BoundingBox->IsBoundActive(EDirection::Right) && (Right < BoundingBoxRight || BoundingBoxLayer > RightLayer))
			{
				Right = BoundingBoxRight;
				RightLayer = BoundingBoxLayer;
			}
		}
	}
	CameraBounds.SetBound(EDirection::Up, Up, UpLayer);
	CameraBounds.SetBound(EDirection::Down, Down, DownLayer);
	CameraBounds.SetBound(EDirection::Left, Left, LeftLayer);
	CameraBounds.SetBound(EDirection::Right, Right, RightLayer);
}

