// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "ButterflyEnemyPawn.generated.h"

UCLASS()
class GRIEF_API AButterflyEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* PathCollisionComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	class USplineComponent* MovementPath = nullptr;

public:
	// Sets default values for this pawn's properties
	AButterflyEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementPathRadius = 100.0f;

	float Speed = 3.0f;
	float Alpha = 0.0f;
};
