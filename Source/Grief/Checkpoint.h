// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

enum class EDirection : uint8;

UCLASS()
class GRIEF_API ACheckpoint : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawn, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SpawnBox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawn, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* DirectionArrow = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox = nullptr;

public:
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	virtual void BeginPlay() override;;

public:
	UFUNCTION(BlueprintPure)
	FVector GetSpawnLocation() const;
	
	UFUNCTION(BlueprintPure)
	EDirection GetSpawnDirection() const;

	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetIndex() const { return Index; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDefault() const { return Default; }

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Checkpoint, meta = (AllowPrivateAccess = "true"))
	uint8 Index = 0;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Checkpoint, meta = (AllowPrivateAccess = "true"))
	bool Default = false;
};
