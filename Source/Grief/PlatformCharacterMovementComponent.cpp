// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformCharacterMovementComponent.h"

#include "GriefCharacter.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UPlatformCharacterMovementComponent::UPlatformCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlatformCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetPlaneConstraintEnabled(true);
	SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
	
	GriefCharacter = Cast<AGriefCharacter>(CharacterOwner);

	if (JumpCurve)
	{
		JumpCurve->GetTimeRange(JumpCurveMin, JumpCurveMax);
	}

	if (KnockbackCurve)
	{
		KnockbackCurve->GetTimeRange(KnockbackCurveMin, KnockbackCurveMax);
	}
}

// Called every frame
void UPlatformCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleJumping(DeltaTime);
	HandleFalling(DeltaTime);
	HandleKnockback(DeltaTime);
}

bool UPlatformCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		/* Don't jump if we can't move up/down */
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.0f)
		{
			if (JumpCurve)
			{
				ReceivingKnockback = false;
				Falling = false;
				Jumping = true;
				JumpCurveTime = JumpCurveMin;
				LastJumpCurveValue = JumpCurve->GetFloatValue(JumpCurveTime);
				
				/* Flying is not influenced by gravity */
				SetMovementMode(MOVE_Flying);

				if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Jumping);
				return true;
			}

			/* Default jump logic */
			Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity);
			SetMovementMode(MOVE_Falling);
			return true;
		}
	}
	
	return false;
}

bool UPlatformCharacterMovementComponent::IsFalling() const
{
	if (JumpCurve)
	{
		return Super::IsFalling() || Jumping || Falling;
	}
	return Super::IsFalling();
}

bool UPlatformCharacterMovementComponent::HasFoundFloor(FFindFloorResult& FindFloorResult, const FVector TraceStart, const FVector TraceEnd) const
{
	const UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();

	FCollisionQueryParams FloorCollisionQueryParams; FloorCollisionQueryParams.AddIgnoredActor(CharacterOwner);
	FCollisionShape CapsuleCollisionShape = FCollisionShape::MakeCapsule(CapsuleComponent->GetScaledCapsuleRadius(), CapsuleComponent->GetScaledCapsuleHalfHeight());

	FHitResult FloorHitResult;

	bool IsBlockingHit = GetWorld()->SweepSingleByProfile(
		FloorHitResult,
		TraceStart,
		TraceEnd,
		CharacterOwner->GetActorRotation().Quaternion(),
		CapsuleComponent->GetCollisionProfileName(),
		CapsuleCollisionShape,
		FloorCollisionQueryParams);

	if (IsBlockingHit)
	{
		FindFloor(TraceStart, FindFloorResult, false, &FloorHitResult);
		return FindFloorResult.IsWalkableFloor() && IsValidLandingSpot(TraceStart, FindFloorResult.HitResult);
	}

	return false;
}

void UPlatformCharacterMovementComponent::HandleJumping(const float DeltaTime)
{
	if (JumpCurve && Jumping)
	{
		JumpCurveTime+=DeltaTime;

		if (JumpCurveTime <= JumpCurveMax)
		{
			const float CurrentJumpCurveValue = JumpCurve->GetFloatValue(JumpCurveTime);
			const float CurrentJumpCurveValueDelta = CurrentJumpCurveValue - LastJumpCurveValue;
			
			LastJumpCurveValue = CurrentJumpCurveValue;

			Velocity.Z = 0.0f;

			float YVelocity = CurrentJumpCurveValueDelta / DeltaTime;

			const FVector ActorLocation = GetActorLocation();
			FVector TargetLocation = ActorLocation + FVector(0.0f, 0.0f, CurrentJumpCurveValueDelta);
			
			UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();
			const FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
			
			if (YVelocity > 0.0f)
			{
				FCollisionQueryParams CeilingCollisionQueryParams; CeilingCollisionQueryParams.AddIgnoredActor(CharacterOwner);
				FCollisionShape CapsuleCollisionShape = FCollisionShape::MakeCapsule(CapsuleComponent->GetScaledCapsuleRadius(), CapsuleComponent->GetScaledCapsuleHalfHeight());
				
				FHitResult CeilingHitResult;
				
				bool IsBlockingHit = GetWorld()->SweepSingleByProfile(
					CeilingHitResult,
					CapsuleLocation,
					TargetLocation,
					CharacterOwner->GetActorRotation().Quaternion(),
					CapsuleComponent->GetCollisionProfileName(),
					CapsuleCollisionShape,
					CeilingCollisionQueryParams);

				if (IsBlockingHit)
				{
					SetFalling();

					Jumping = false;
					CharacterOwner->ResetJumpState();

					Velocity.Z = 0.0f;
					
					TargetLocation = ActorLocation;
				}
			}
			
			if (YVelocity < 0.0f)
			{

				if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Falling);
				
				FFindFloorResult FindFloorResult;
				const bool FoundFloor = HasFoundFloor(FindFloorResult, CapsuleLocation, TargetLocation);
				const float FloorDistance = FindFloorResult.GetDistanceToFloor();

				if (FoundFloor)
				{
					if (FloorDistance < FMath::Abs(CurrentJumpCurveValueDelta))
					{
						TargetLocation = CapsuleLocation + FVector(0.0f, 0.0f, CurrentJumpCurveValueDelta);
					}
				
					SetMovementMode(MOVE_Walking);
					if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Grounded);

					Jumping = false;
					CharacterOwner->ResetJumpState();
				}
			}

			GetOwner()->SetActorLocation(TargetLocation, true);
			
			//FLatentActionInfo LatentActionInfo; LatentActionInfo.CallbackTarget = this;
			//UKismetSystemLibrary::MoveComponentTo(Cast<USceneComponent>(CapsuleComponent), TargetLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Type::Move, LatentActionInfo);
		}
		else
		{
			const UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();
			const FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
			
			FFindFloorResult FindFloorResult;
			FindFloor(CapsuleLocation, FindFloorResult, false);
			
			if (FindFloorResult.IsWalkableFloor() && IsValidLandingSpot(CapsuleLocation, FindFloorResult.HitResult))
			{
				SetMovementMode(MOVE_Walking);
				if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Grounded);
			}
			else SetFalling();

			Jumping = false;
			CharacterOwner->ResetJumpState();
		}
	}
}

void UPlatformCharacterMovementComponent::HandleFalling(const float DeltaTime)
{
	if (FallCurve)
	{
		if (Falling)
		{
			FallCurveTime+=DeltaTime;
		
			const float CurrentFallCurveValue = FallCurve->GetFloatValue(FallCurveTime);
			const float CurrentFallCurveValueDelta = CurrentFallCurveValue - LastFallCurveValue;
		
			LastFallCurveValue = CurrentFallCurveValue;

			//Velocity.Z = CurrentFallCurveValueDelta / DeltaTime;
			Velocity.Z = 0.0f;

			const FVector CapsuleLocation = CharacterOwner->GetCapsuleComponent()->GetComponentLocation();
			FVector TargetLocation = CapsuleLocation + FVector(0.0f, 0.0f, CurrentFallCurveValueDelta);

			FFindFloorResult FindFloorResult;
			const bool FoundFloor = HasFoundFloor(FindFloorResult, CapsuleLocation, TargetLocation);
			const float FloorDistance = FindFloorResult.GetDistanceToFloor();

			if (FoundFloor)
			{
				if (FloorDistance < FMath::Abs(CurrentFallCurveValueDelta))
				{
					TargetLocation = CapsuleLocation - FVector(0.0f, 0.0f, FloorDistance);

					Falling = false;
					Velocity = FVector::ZeroVector;
					SetMovementMode(MOVE_Walking);
					if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Grounded);
				}
			}
		
			//FLatentActionInfo LatentActionInfo; LatentActionInfo.CallbackTarget = this;
			//USceneComponent* CapsuleComponent = Cast<USceneComponent>(CharacterOwner->GetCapsuleComponent());

			GetOwner()->SetActorLocation(TargetLocation, true);
			//UKismetSystemLibrary::MoveComponentTo(CapsuleComponent, TargetLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Type::Move, LatentActionInfo);
		}
		else if (MovementMode == MOVE_Falling) SetFalling();
	}
}

void UPlatformCharacterMovementComponent::HandleKnockback(const float DeltaTime)
{
	if (KnockbackCurve && ReceivingKnockback)
	{
		KnockbackCurveTime+=DeltaTime;

		if (KnockbackCurveTime <= KnockbackCurveMax)
		{
			const float CurrentKnockbackCurveValue = KnockbackCurve->GetFloatValue(KnockbackCurveTime);
			const float CurrentKnockbackCurveValueDelta = CurrentKnockbackCurveValue - LastKnockbackCurveValue;
		
			LastKnockbackCurveValue = CurrentKnockbackCurveValue;

			Velocity.Z = 0.0f;

			float YVelocity = CurrentKnockbackCurveValueDelta / DeltaTime;

			UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();
			const FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();

			const FVector ActorLocation = GetActorLocation();
			FVector TargetLocation = CapsuleLocation + KnockbackVector * (CurrentKnockbackCurveValueDelta * KnockbackVelocity);

			if (YVelocity > 0.0f)
			{
				FCollisionQueryParams CollisionQueryParams; CollisionQueryParams.AddIgnoredActor(CharacterOwner);
				FCollisionShape CapsuleCollisionShape = FCollisionShape::MakeCapsule(CapsuleComponent->GetScaledCapsuleRadius(), CapsuleComponent->GetScaledCapsuleHalfHeight());
				
				FHitResult HitResult;
				
				bool IsBlockingHit = GetWorld()->SweepSingleByProfile(
					HitResult,
					CapsuleLocation,
					TargetLocation,
					CharacterOwner->GetActorRotation().Quaternion(),
					CapsuleComponent->GetCollisionProfileName(),
					CapsuleCollisionShape,
					CollisionQueryParams);

				if (IsBlockingHit)
				{
					FFindFloorResult FindFloorResult;
					FindFloor(CapsuleLocation, FindFloorResult, false);
			
					if (!FindFloorResult.IsWalkableFloor() || !IsValidLandingSpot(CapsuleLocation, FindFloorResult.HitResult))
					{
						SetFalling();
					}

					ReceivingKnockback = false;

					Velocity.Z = 0.0f;
					
					TargetLocation = ActorLocation;
				}
			}

			GetOwner()->SetActorLocation(TargetLocation, true);
			//FLatentActionInfo LatentActionInfo; LatentActionInfo.CallbackTarget = this;
			//UKismetSystemLibrary::MoveComponentTo(CapsuleComponent, TargetLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Type::Move, LatentActionInfo);
		}
		else
		{
			const UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();
			const FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
			
			FFindFloorResult FindFloorResult;
			FindFloor(CapsuleLocation, FindFloorResult, false);
			
			if (FindFloorResult.IsWalkableFloor() && IsValidLandingSpot(CapsuleLocation, FindFloorResult.HitResult))
			{
				SetMovementMode(MOVE_Walking);
				if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Grounded);
			}
			else SetFalling();

			ReceivingKnockback = false;
		}
	}
}

void UPlatformCharacterMovementComponent::SetFalling()
{
	if (FallCurve)
	{
		Jumping = false;
		ReceivingKnockback = false;
		Falling = true;
		FallCurveTime = 0.0f;
		LastFallCurveValue = 0.0f;
		Velocity.Z = 0.0f;
		
		SetMovementMode(MOVE_Flying);
		if (GriefCharacter) GriefCharacter->SetPlatformerMovementMode(EPlatformerMovementMode::Falling);
	}
	else
	{
		SetMovementMode(MOVE_Falling);
	}
}

void UPlatformCharacterMovementComponent::Knockback(FVector InKnockbackVector, float InKnockbackVelocity)
{
	if (KnockbackCurve)
	{
		KnockbackVector = InKnockbackVector;
		KnockbackVelocity = InKnockbackVelocity;
		
		Falling = false;
		Jumping = false;
		ReceivingKnockback = true;
		KnockbackCurveTime = KnockbackCurveMin;
		LastKnockbackCurveValue = KnockbackCurve->GetFloatValue(KnockbackCurveTime);
				
		/* Flying is not influenced by gravity */
		SetMovementMode(MOVE_Flying);
	}
}

