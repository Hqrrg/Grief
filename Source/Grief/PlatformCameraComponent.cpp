// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformCameraComponent.h"

#include "CameraBoundingBox.h"
#include "PlayerPawn.h"
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

	// Default biases and interp speeds
	float YTargetBias = 0.0f;
	float ZTargetBias = 0.0f;
	float YInterpSpeed = 5.0f;
	float ZInterpSpeed = 10.0f;
	
	if (APlayerPawn* PlayerCharacter = Cast<APlayerPawn>(GetOwner()))
	{
		EDirection MovementDirection = PlayerCharacter->GetMovementDirection();

		// Y-Bias based on movement direction
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

		EPlatformMovementMode PlatformerMovementMode = PlayerCharacter->GetPlatformMovementComponent()->GetMovementMode();

		// Z-Bias based on movement mode
		switch (PlatformerMovementMode)
		{
		case EPlatformMovementMode::Jumping:
			ZTargetBias = -1.0f;
			break;
			
		case EPlatformMovementMode::Falling:
			ZTargetBias = 1.0f;
			break;
			
		default:
			break;
		}
	}
	// Multiply bias (direction) by amount
	YTargetBias*=YawMovementBias;
	ZTargetBias*=PitchMovementBias;

	// X Offset / Zoom
	float XTargetBias = CameraZoom;
	float XInterpSpeed = 5.0f;

	if (CurrentCameraBoundingBox) XTargetBias+=CurrentCameraBoundingBox->GetOffset();
	if (CurrentCameraBoundingBox) XInterpSpeed = CurrentCameraBoundingBox->GetOffsetInterpSpeed();

	// Create camera target location
	float TargetX = ActorLocation.X-XTargetBias;
	float TargetY = ActorLocation.Y+YTargetBias;
	float TargetZ = ActorLocation.Z+ZTargetBias;
	
	if (TargetY > CameraBounds.Right) TargetY = CameraBounds.Right;
	if (TargetY < CameraBounds.Left) TargetY = CameraBounds.Left;
	if (TargetZ > CameraBounds.Up) TargetZ = CameraBounds.Up;
	if (TargetZ < CameraBounds.Down) TargetZ = CameraBounds.Down;

	// Set interp speeds to new target location
	if (FMath::IsNearlyEqual(GetOwner()->GetVelocity().Length(), 0.0f, 0.5f))
	{
		YInterpSpeed = 2.5f;
		ZInterpSpeed = 5.0f;
	}

	const float InterpTargetX = FMath::FInterpTo(CameraLocation.X, TargetX, DeltaTime, XInterpSpeed);
	const float InterpTargetY = FMath::FInterpTo(CameraLocation.Y, TargetY, DeltaTime, YInterpSpeed);
	const float InterpTargetZ = FMath::FInterpTo(CameraLocation.Z, TargetZ, DeltaTime, ZInterpSpeed);

	const FVector TargetLocation = FVector(InterpTargetX, InterpTargetY, InterpTargetZ);
	// Update location
	Camera->SetWorldLocation(TargetLocation);
}

void UPlatformCameraComponent::AddCameraBoundingBox(ACameraBoundingBox* CameraBoundingBox)
{
	CameraBoundingBoxes.AddUnique(CameraBoundingBox);
	CurrentCameraBoundingBox = CameraBoundingBox;
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
	const FVector TargetLocation = FVector(ActorLocation.X - CameraZoom, ActorLocation.Y, ActorLocation.Z);
	
	Camera->SetWorldLocation(TargetLocation);
}

void UPlatformCameraComponent::UpdateCameraBounds()
{
	// Don't continue if there are no active bounds
	if (CameraBoundingBoxes.IsEmpty()) return;

	// Update current bounds to the last one entered
	CurrentCameraBoundingBox = CameraBoundingBoxes[CameraBoundingBoxes.Num()-1];
	
	int32 LayerUp = INT_MIN;
	int32 LayerDown = INT_MIN;
	int32 LayerLeft = INT_MIN;
	int32 LayerRight = INT_MIN;

	float Up = FLT_MAX;
	float Down = FLT_MIN;
	float Left = FLT_MIN;
	float Right = FLT_MAX;

	// Loop through bounding boxes and calculate which bounds to use
	for (uint8 Index = 0; Index < CameraBoundingBoxes.Num(); Index++)
	{
		if (ACameraBoundingBox* BoundingBox = CameraBoundingBoxes[Index])
		{
			const float BoundingBoxUp = BoundingBox->GetBound(EDirection::Up);
			const float BoundingBoxDown = BoundingBox->GetBound(EDirection::Down);
			const float BoundingBoxLeft = BoundingBox->GetBound(EDirection::Left);
			const float BoundingBoxRight = BoundingBox->GetBound(EDirection::Right);

			const uint8 BoundingBoxLayer = BoundingBox->GetLayer();
			
			if (BoundingBox->IsBoundActive(EDirection::Up)) UpdateBound(Up, BoundingBoxUp, LayerUp, BoundingBoxLayer);
			if (BoundingBox->IsBoundActive(EDirection::Down)) UpdateBound(Down, BoundingBoxDown, LayerDown, BoundingBoxLayer);
			if (BoundingBox->IsBoundActive(EDirection::Left)) UpdateBound(Left, BoundingBoxLeft, LayerLeft, BoundingBoxLayer);
			if (BoundingBox->IsBoundActive(EDirection::Right)) UpdateBound(Right, BoundingBoxRight, LayerRight, BoundingBoxLayer);
		}
	}

	// Update bounds
	CameraBounds.SetBound(EDirection::Up, Up, LayerUp);
	CameraBounds.SetBound(EDirection::Down, Down, LayerDown);
	CameraBounds.SetBound(EDirection::Left, Left, LayerLeft);
	CameraBounds.SetBound(EDirection::Right, Right, LayerRight);
}

void UPlatformCameraComponent::UpdateBound(float& Bound, const float InBound, int32& Layer, const int32 InLayer)
{
	// Dont update if current layer is above new layer
	if (Layer > InLayer) return;
	// Update if new layer is above current layer
	if (Layer < InLayer)
	{
		Bound = InBound;
		Layer = InLayer;
		return;
	}
	// If layers are identical, choose the smallest (absolute) bound
	Bound = FMath::Abs(Bound) < FMath::Abs(InBound) ? Bound : InBound;
}

void UPlatformCameraComponent::ResetComponent()
{
	CurrentCameraBoundingBox = nullptr;
	CameraBoundingBoxes.Empty();
	CameraBounds = FCameraBounds();
}

