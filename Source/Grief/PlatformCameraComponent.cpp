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

	
	if (TargetY > CameraBounds.Right) TargetY = CameraBounds.Right;
	if (TargetY < CameraBounds.Left) TargetY = CameraBounds.Left;
	if (TargetZ > CameraBounds.Up) TargetZ = CameraBounds.Up;
	if (TargetZ < CameraBounds.Down) TargetZ = CameraBounds.Down;

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
	if (CameraBoundingBoxes.IsEmpty()) return;
	
	int32 LayerUp = INT_MIN;
	int32 LayerDown = INT_MIN;
	int32 LayerLeft = INT_MIN;
	int32 LayerRight = INT_MIN;

	float Up = FLT_MAX;
	float Down = FLT_MIN;
	float Left = FLT_MIN;
	float Right = FLT_MAX;
	
	for (uint8 Index = 0; Index < CameraBoundingBoxes.Num(); Index++)
	{
		if (ACameraBoundingBox* BoundingBox = CameraBoundingBoxes[Index])
		{
			const float BoundingBoxUp = BoundingBox->GetBound(EDirection::Up);
			const float BoundingBoxDown = BoundingBox->GetBound(EDirection::Down);
			const float BoundingBoxLeft = BoundingBox->GetBound(EDirection::Left);
			const float BoundingBoxRight = BoundingBox->GetBound(EDirection::Right);

			const int32 BoundingBoxLayer = BoundingBox->GetLayer();
			
			if (BoundingBox->IsBoundActive(EDirection::Up)) UpdateBound(Up, BoundingBoxUp, LayerUp, BoundingBoxLayer);
			if (BoundingBox->IsBoundActive(EDirection::Down)) UpdateBound(Down, BoundingBoxDown, LayerDown, BoundingBoxLayer);
			if (BoundingBox->IsBoundActive(EDirection::Left)) UpdateBound(Left, BoundingBoxLeft, LayerLeft, BoundingBoxLayer);
			if (BoundingBox->IsBoundActive(EDirection::Right)) UpdateBound(Right, BoundingBoxRight, LayerRight, BoundingBoxLayer);
		}
	}
	
	CameraBounds.SetBound(EDirection::Up, Up, LayerUp);
	CameraBounds.SetBound(EDirection::Down, Down, LayerDown);
	CameraBounds.SetBound(EDirection::Left, Left, LayerLeft);
	CameraBounds.SetBound(EDirection::Right, Right, LayerRight);
}

void UPlatformCameraComponent::UpdateBound(float& Bound, const float InBound, int32& Layer, const int32 InLayer)
{
	if (Layer > InLayer) return;
	
	if (Layer < InLayer)
	{
		Bound = InBound;
		Layer = InLayer;
		return;
	}

	Bound = FMath::Abs(Bound) < FMath::Abs(InBound) ? Bound : InBound;
}

