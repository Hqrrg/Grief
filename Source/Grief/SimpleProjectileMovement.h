// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SimpleProjectileMovement.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectileHit, AActor*, HitActor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIEF_API USimpleProjectileMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USimpleProjectileMovement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetMovementVector(FVector InMovementVector);

	UPROPERTY(BlueprintAssignable)
	FProjectileHit OnProjectileHit;

private:
	void HandleMovement(float DeltaTime);
	bool CheckCollided(FHitResult& HitResult, FVector TraceEnd);
	void HandleProjectileHit(AActor* HitActor);
	void ResetMovement();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float Speed = 2400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MovementCurve = nullptr;
	
private:
	UPROPERTY()
	USceneComponent* UpdatedComponent = nullptr;

	UPROPERTY()
	UShapeComponent* CollisionComponent = nullptr;
	
	FVector MovementVector = FVector::ZeroVector;

	FHitResult CollisionHitResult;

	bool ShouldMove = true;
};
