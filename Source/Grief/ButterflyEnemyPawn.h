// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "ButterflyEnemyPawn.generated.h"

UENUM(BlueprintType)
enum class EButterflyPathType : uint8
{
	Figure8,
	Circle
};

UCLASS()
class GRIEF_API AButterflyEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PathCollision = nullptr;
	
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EButterflyPathType ButterflyPathType = EButterflyPathType::Figure8;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementPathRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed = 3.0f;
	
	float Alpha = 0.0f;
};
