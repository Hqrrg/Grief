// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPawnFunctionLibrary.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

#include "Engine/LatentActionManager.h"


void UAIPawnFunctionLibrary::AIPawnMoveToLocation(AAIController* InController, FLatentActionInfo LatentInfo, EAIPawnMoveToLocationInput InputPins, EAIPawnMoveToLocationOutput& OutputPins, FVector InLocation, float InAcceptanceRadius, float InTimeout, bool CanFly)
{
	UWorld* World = GEngine->GetWorldFromContextObject(InController, EGetWorldErrorMode::ReturnNull);

#if WITH_EDITOR
	if (!World && GEditor)
	{
		//World = GEditor->GetEditorWorldContext().World();
	}
#endif

	if (!World) return;

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	FAIPawnMoveToLocation* ExistingAction = LatentActionManager.FindExistingAction<FAIPawnMoveToLocation>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	if (InputPins == EAIPawnMoveToLocationInput::In)
	{
		if (!ExistingAction)
		{
			FAIPawnMoveToLocation* Action = new FAIPawnMoveToLocation(LatentInfo, OutputPins, InController, InLocation, InAcceptanceRadius, InTimeout, CanFly);
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
		}

		if (ExistingAction)
		{
			ExistingAction->AcceptanceRadius = InAcceptanceRadius;
			ExistingAction->TargetLocation = InLocation;
		}
	}
}

void FAIPawnMoveToLocation::UpdateOperation(FLatentResponse& Response)
{
	// Track total elapsed time since operation began
	ElapsedTime+=Response.ElapsedTime();

	// If it's been X (FailCheckRate) ticks since the player has moved, then task should fail.
	if (TickCount % FailCheckRate == 0) bShouldFail = LastPosition == Pawn->GetActorLocation();
	// Increment tick count & set last position to pawn position this tick
	TickCount++;
	LastPosition = Pawn->GetActorLocation();
	
	FVector PawnLocation = Pawn->GetActorLocation();
	FVector DirectionVector = TargetLocation - PawnLocation;
	FVector DirectionVector2D = DirectionVector.GetSafeNormal();
	float DistanceToTarget = DirectionVector.Length();

	// Execute failure pin if should fail or timed out
	if (bShouldFail || Timeout != -1.0f && ElapsedTime >= Timeout)
	{
		Output = EAIPawnMoveToLocationOutput::Failure;
		Response.FinishAndTriggerIf(true, LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
		return;
	}
	// Remove Z axis from distance calculation if pawn cannot fly
	if (!bCanFly) DistanceToTarget = FVector(DirectionVector.X, DirectionVector.Y, 0.0f).Length();
	
	// Execute success pin if within acceptance radius
	if (DistanceToTarget <= AcceptanceRadius || DistanceToTarget <= 10.0f)
	{
		Output = EAIPawnMoveToLocationOutput::Success;
		Response.FinishAndTriggerIf(true, LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
		return;
	}
	// Move
	float DirectionY = DirectionVector2D.Y;
	float DirectionZ = DirectionVector2D.Z;

	const FVector YawDirection = FVector(0.0f, DirectionY, 0.0f);
	const FVector PitchDirection = FVector(0.0f, 0.0f, DirectionZ);

	if (bCanFly) Pawn->AddMovementInput(YawDirection + PitchDirection, 1.0f);
	else Pawn->AddMovementInput(YawDirection, 1.0f);

	// Execute out pin every call if success/failure was not called
	Output = EAIPawnMoveToLocationOutput::Out;
	Response.TriggerLink(LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
}
