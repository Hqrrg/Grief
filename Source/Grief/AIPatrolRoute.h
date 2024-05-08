// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPatrolRoute.generated.h"

UCLASS()
class GRIEF_API AAIPatrolRoute : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	FVector GetLocationAtIndex(uint8 Index);

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetRouteLength() const { return Route.Num(); }
	
private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Route, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FVector> Route;
	
};
