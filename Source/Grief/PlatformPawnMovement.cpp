// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformPawnMovement.h"

#include "Components/BoxComponent.h"


// Sets default values for this component's properties
UPlatformPawnMovement::UPlatformPawnMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlatformPawnMovement::InitializeComponent()
{
	Super::InitializeComponent();

	CollisionComponent = Cast<UBoxComponent>(UpdatedComponent);
}

void UPlatformPawnMovement::BeginPlay()
{
	Super::BeginPlay();
	
	if (JumpCurve)
	{
		JumpCurve->GetTimeRange(MinJumpCurveTime, MaxJumpCurveTime);
	}

	if (KnockbackCurve)
	{
		KnockbackCurve->GetTimeRange(MinKnockbackCurveTime, MaxKnockbackCurveTime);
	}
}

// Called every frame
void UPlatformPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	HandleJumping(DeltaTime);
	HandleFalling(DeltaTime);
	HandleKnockback(DeltaTime);
	
	if (!IsGrounded() && !IsJumping() && !IsReceivingKnockback() && !IsFalling()) SetFalling();
}

void UPlatformPawnMovement::Jump()
{
	if (IsJumping()) return;
	
	const bool IsFinalJump = CurrentJumpCount == MaxJumpsAllowed-1;
	
	if (CanJump())
	{
		JumpHeight = MaxJumpHeight / (1 + IsFinalJump);
		
		CurrentJumpCurveTime = MinJumpCurveTime;
		PreviousJumpCurveValue = JumpCurve->GetFloatValue(CurrentJumpCurveTime);
		
		Falling = false;
		Jumping = true;
		CurrentJumpCount++;
		SetMovementMode(EPlatformMovementMode::Jumping);
	}
}

void UPlatformPawnMovement::StopJumping()
{
	const bool IsFinalJump = CurrentJumpCount == MaxJumpsAllowed-1;
	
	if (IsJumping())
	{
		if (IsFinalJump) SetFalling();
	}
}

void UPlatformPawnMovement::SetFalling()
{
	if (FallCurve)
	{
		CurrentFallCurveTime = 0.0f;
		PreviousFallCurveValue = FallCurve->GetFloatValue(CurrentFallCurveTime);

		ReceivingKnockback = false;
		Jumping = false;
		Falling = true;
		SetMovementMode(EPlatformMovementMode::Falling);
	}
}

void UPlatformPawnMovement::Knockback(FVector InKnockbackVector, float InKnockbackVelocity)
{
	if (KnockbackCurve)
	{
		KnockbackVector = InKnockbackVector;
		KnockbackVelocity = InKnockbackVelocity;

		Falling = false;
		Jumping = false;
		ReceivingKnockback = true;
		CurrentKnockbackCurveTime = MinKnockbackCurveTime;
		PreviousKnockbackCurveValue = KnockbackCurve->GetFloatValue(CurrentKnockbackCurveTime);

		SetMovementMode(EPlatformMovementMode::Walking);
	}
}

void UPlatformPawnMovement::SetMovementMode(const EPlatformMovementMode InMovementMode)
{
	MovementMode = InMovementMode;
	MovementModeUpdated.Broadcast();
}

bool UPlatformPawnMovement::IsFalling() const
{
	return Falling;
}

bool UPlatformPawnMovement::IsGrounded()
{
	FHitResult* Ground = FindGround(2.0f);
	return Ground ? true : false;
}

FHitResult* UPlatformPawnMovement::FindGround(float InDistance)
{
	
	FVector BoxLocation = CollisionComponent->GetComponentLocation();
	FVector BoxExtent = CollisionComponent->GetScaledBoxExtent();
	
	FVector Start = FVector(BoxLocation.X, BoxLocation.Y, BoxLocation.Z-BoxExtent.Z);
	FVector End = Start - FVector(0.0f, 0.0f, InDistance);
	
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(BoxExtent.X, BoxExtent.Y, 0.0f) / 2);
				
	FCollisionQueryParams CollisionQueryParams; CollisionQueryParams.AddIgnoredActor(PawnOwner);
	FHitResult* HitResult = new FHitResult();
				
	bool IsBlocking = GetWorld()->SweepSingleByProfile(
		*HitResult,
		Start,
		End,
		PawnOwner->GetActorRotation().Quaternion(),
		CollisionComponent->GetCollisionProfileName(),
		CollisionShape,
		CollisionQueryParams);

	return IsBlocking ? HitResult : nullptr;
}

bool UPlatformPawnMovement::CanJump()
{
	return JumpCurve && (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.0f) &&
		(!IsFalling() || CurrentJumpCount < MaxJumpsAllowed) && !ReceivingKnockback; 
}

void UPlatformPawnMovement::Landed()
{
	ReceivingKnockback = false;
	Jumping = false;
	Falling = false;
	CurrentJumpCount = 0;
	SetMovementMode(EPlatformMovementMode::Walking);
}

void UPlatformPawnMovement::HandleJumping(float DeltaTime)
{
	if (JumpCurve && Jumping)
	{
		CurrentJumpCurveTime+=DeltaTime;

		if (CurrentJumpCurveTime <= MaxJumpCurveTime)
		{
			float CurrentJumpCurveValue = JumpCurve->GetFloatValue(CurrentJumpCurveTime) * JumpHeight;
			float CurrentJumpCurveValueDelta = CurrentJumpCurveValue - PreviousJumpCurveValue;

			PreviousJumpCurveValue = CurrentJumpCurveValue;
			
			float ZVelocity = CurrentJumpCurveValueDelta / DeltaTime;

			FVector ActorLocation = GetActorLocation();
			FVector TargetLocation = ActorLocation + FVector(0.0f, 0.0f, CurrentJumpCurveValueDelta);

			if (ZVelocity > 0.0f)
			{
				FVector BoxLocation = CollisionComponent->GetComponentLocation();
				FVector BoxExtent = CollisionComponent->GetScaledBoxExtent();
	
				FVector Start = BoxLocation;
				FVector End = TargetLocation;
	
				FCollisionShape CollisionShape = FCollisionShape::MakeBox(BoxExtent / 2);
				
				FCollisionQueryParams CollisionQueryParams; CollisionQueryParams.AddIgnoredActor(PawnOwner);
				FHitResult* HitResult = new FHitResult();
				
				bool IsBlocking = GetWorld()->SweepSingleByProfile(
					*HitResult,
					Start,
					End,
					PawnOwner->GetActorRotation().Quaternion(),
					CollisionComponent->GetCollisionProfileName(),
					CollisionShape,
					CollisionQueryParams);

				if (IsBlocking)
				{
					TargetLocation = ActorLocation;
					SetFalling();
				}
			}
			else if (ZVelocity < 0.0f)
			{
				SetMovementMode(EPlatformMovementMode::Falling);

				FHitResult* Ground = FindGround(ActorLocation.Z-TargetLocation.Z);
				
				if (Ground)
				{
					if (Ground->Distance < ActorLocation.Z-TargetLocation.Z)
					{
						TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, Ground->Location.Z);
						Landed();
					}
				}
			}
			
			PawnOwner->SetActorLocation(TargetLocation, true);
		}
		else
		{
			if (IsGrounded()) Landed();
			else SetFalling();
		}
	}
}

void UPlatformPawnMovement::HandleFalling(float DeltaTime)
{
	if (FallCurve && Falling)
	{
		CurrentFallCurveTime+=DeltaTime;

		float CurrentFallCurveValue = FallCurve->GetFloatValue(CurrentFallCurveTime) * JumpHeight;
		float CurrentFallCurveValueDelta = CurrentFallCurveValue - PreviousFallCurveValue;

		PreviousJumpCurveValue = CurrentFallCurveValue;
		
		FVector ActorLocation = GetActorLocation();
		FVector TargetLocation = ActorLocation + FVector(0.0f, 0.0f, CurrentFallCurveValueDelta);

		FHitResult* Ground = FindGround(ActorLocation.Z-TargetLocation.Z);
		
		if (Ground)
		{
			if (Ground->Distance < ActorLocation.Z-TargetLocation.Z)
			{
				TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, Ground->Location.Z);
				Landed();
			}
		}

		PawnOwner->SetActorLocation(TargetLocation, true);
	}
}

void UPlatformPawnMovement::HandleKnockback(float DeltaTime)
{
	if (KnockbackCurve && ReceivingKnockback)
	{
		CurrentKnockbackCurveTime+=DeltaTime;

		if (CurrentKnockbackCurveTime <= MaxKnockbackCurveTime)
		{
			const float CurrentKnockbackCurveValue = KnockbackCurve->GetFloatValue(CurrentKnockbackCurveTime);
			const float CurrentKnockbackCurveValueDelta = CurrentKnockbackCurveValue - PreviousKnockbackCurveValue;
		
			PreviousKnockbackCurveValue = CurrentKnockbackCurveValue;

			float YVelocity = CurrentKnockbackCurveValueDelta / DeltaTime;

			const FVector ActorLocation = GetActorLocation();
			FVector TargetLocation = ActorLocation + KnockbackVector * (CurrentKnockbackCurveValueDelta * KnockbackVelocity);

			if (YVelocity > 0.0f)
			{
				FCollisionQueryParams CollisionQueryParams; CollisionQueryParams.AddIgnoredActor(PawnOwner);
				FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionComponent->GetScaledBoxExtent() / 2);
				
				FHitResult* HitResult = new FHitResult();
				
				bool IsBlocking = GetWorld()->SweepSingleByProfile(
					*HitResult,
					CollisionComponent->GetComponentLocation(),
					TargetLocation,
					PawnOwner->GetActorRotation().Quaternion(),
					CollisionComponent->GetCollisionProfileName(),
					CollisionShape,
					CollisionQueryParams);

				if (IsBlocking)
				{
					FHitResult* Ground = FindGround(2.0f);

					if (Ground)
					{
						if (Ground->Distance < ActorLocation.Z-2.0f)
						{
							TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, Ground->Location.Z);
							Landed();
						}
					}
				}
			}

			PawnOwner->SetActorLocation(TargetLocation, true);
		}
		else
		{
			if (IsGrounded()) Landed();
			else SetFalling();
		}
	}
}

