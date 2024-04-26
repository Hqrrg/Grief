// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPawnFunctionLibrary.h"

#include "Editor.h"
#include "Engine/LatentActionManager.h"


void UAIPawnFunctionLibrary::AIPawnMoveToLocation(AAIController* InController, FLatentActionInfo LatentInfo, EAIPawnMoveToLocationInput InputPins, EAIPawnMoveToLocationOutput& OutputPins, FVector InLocation, float InAcceptanceRadius)
{
	UWorld* World = GEngine->GetWorldFromContextObject(InController, EGetWorldErrorMode::ReturnNull);

	if (!World && GEditor)
	{
		//World = GEditor->GetEditorWorldContext().World();
	}

	if (!World) return;

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	FAIPawnMoveToLocation* ExistingAction = LatentActionManager.FindExistingAction<FAIPawnMoveToLocation>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	if (InputPins == EAIPawnMoveToLocationInput::In)
	{
		if (!ExistingAction)
		{
			FAIPawnMoveToLocation* Action = new FAIPawnMoveToLocation(LatentInfo, OutputPins, InController, InLocation, InAcceptanceRadius);
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
	FVector PawnLocation = Pawn->GetActorLocation();
	FVector DirectionVector = TargetLocation - PawnLocation;
	float DistanceToTarget = DirectionVector.Length();
	
	if (DistanceToTarget <= AcceptanceRadius || FVector::PointsAreNear(PawnLocation, TargetLocation, 10.0f))
	{
		Output = EAIPawnMoveToLocationOutput::Success;
		Response.FinishAndTriggerIf(true, LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
		return;
	}

	float DirectionY = DirectionVector.Y;

	if (DirectionY < 0.0f) DirectionY = -1.0f;
	if (DirectionY > 0.0f) DirectionY = 1.0f;

	const FVector WorldDirection = FVector(0.0f, DirectionY, 0.0f);
	
	Pawn->AddMovementInput(WorldDirection, 1.0f);

	if (Pawn->GetVelocity().Length() == 0)
	{
		Output = EAIPawnMoveToLocationOutput::Failure;
		Response.FinishAndTriggerIf(true, LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
		return;
	}
	
	Output = EAIPawnMoveToLocationOutput::Out;
	Response.TriggerLink(LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
}
