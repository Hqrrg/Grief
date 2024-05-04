// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPawnFunctionLibrary.h"

#include "Editor.h"
#include "Engine/LatentActionManager.h"


void UAIPawnFunctionLibrary::AIPawnMoveToLocation(AAIController* InController, FLatentActionInfo LatentInfo, EAIPawnMoveToLocationInput InputPins, EAIPawnMoveToLocationOutput& OutputPins, FVector InLocation, float InAcceptanceRadius, float InTimeout, bool CanFly)
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
	ElapsedTime+=Response.ElapsedTime();
	
	FVector PawnLocation = Pawn->GetActorLocation();
	FVector DirectionVector = TargetLocation - PawnLocation;
	float DistanceToTarget = DirectionVector.Length();
	
	if (Timeout != -1.0f && ElapsedTime >= Timeout)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Failure")));
		Output = EAIPawnMoveToLocationOutput::Failure;
		Response.FinishAndTriggerIf(true, LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
		return;
	}
	
	if (!bCanFly)
	{
		DistanceToTarget = FVector(DirectionVector.X, DirectionVector.Y, 0.0f).Length();
	}
	
	if (DistanceToTarget <= AcceptanceRadius || DistanceToTarget <= 10.0f)
	{
		Output = EAIPawnMoveToLocationOutput::Success;
		Response.FinishAndTriggerIf(true, LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
		return;
	}
	
	float DirectionY = DirectionVector.Y;
	float DirectionZ = DirectionVector.Z;

	if (DirectionY <= -0.5f) DirectionY = -1.0f;
	else if (DirectionY >= 0.5f) DirectionY = 1.0f;

	if (DirectionZ <= -0.5f) DirectionZ = -1.0f;
	else if (DirectionZ >= 0.5f) DirectionZ = 1.0f;

	const FVector YawDirection = FVector(0.0f, DirectionY, 0.0f);
	const FVector PitchDirection = FVector(0.0f, 0.0f, DirectionZ);

	if (bCanFly)
	{
		Pawn->AddMovementInput(YawDirection + PitchDirection, 1.0f);
	}
	else
	{
		Pawn->AddMovementInput(YawDirection, 1.0f);
	}
	
	Output = EAIPawnMoveToLocationOutput::Out;
	Response.TriggerLink(LatentActionInfo.ExecutionFunction, LatentActionInfo.Linkage, LatentActionInfo.CallbackTarget);
}
