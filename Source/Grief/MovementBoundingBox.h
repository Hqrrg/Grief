// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementBoundingBox.generated.h"

UCLASS(NotBlueprintable)
class GRIEF_API AMovementBoundingBox : public AActor
{
	GENERATED_BODY()

	/* Navigation Box Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* NavigationBox;

public:
	// Sets default values for this actor's properties
	AMovementBoundingBox();

protected:
	/* Called when this actor is spawned */
	virtual void BeginPlay() override;

public:
	/* Return whether a provided location falls within the navigation box */
	bool IsLocationWithinArea(FVector& Location);

private:
	UPROPERTY()
	FVector MinLocation;

	UPROPERTY()
	FVector MaxLocation;
};
