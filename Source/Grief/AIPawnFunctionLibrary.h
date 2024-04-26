// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LatentActions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIPawnFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EAIPawnMoveToLocationInput : uint8
{
	In
};

UENUM(BlueprintType)
enum class EAIPawnMoveToLocationOutput : uint8
{
	Out,
	Success,
	Failure
};

UCLASS()
class GRIEF_API UAIPawnFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Controller", Latent, LatentInfo = "LatentInfo", ExpandEnumAsExecs = "InputPins, OutputPins"), Category = "AI")
	static void AIPawnMoveToLocation(AAIController* InController, FLatentActionInfo LatentInfo, EAIPawnMoveToLocationInput InputPins, EAIPawnMoveToLocationOutput& OutputPins,
		FVector InLocation, float InAcceptanceRadius = -1);
};

class FAIPawnMoveToLocation : public FPendingLatentAction
{
public:
	FVector TargetLocation;
	
	float AcceptanceRadius;
	
	AAIController* Controller;

	APawn* Pawn;
	
public:
	bool bFirstCall = true;

public:
	FLatentActionInfo LatentActionInfo;
	
	EAIPawnMoveToLocationOutput& Output;

public:
	FAIPawnMoveToLocation(FLatentActionInfo& LatentInfo, EAIPawnMoveToLocationOutput& OutputPins, AAIController* InController, FVector InLocation, float InAcceptanceRadius)
		: LatentActionInfo(LatentInfo)
		, Output(OutputPins)
		, Controller(InController)
		, Pawn(InController->GetPawn())
		, TargetLocation(InLocation)
		, AcceptanceRadius(InAcceptanceRadius)
	{
		Output = EAIPawnMoveToLocationOutput::Out;
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;
};
