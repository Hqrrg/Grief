// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"


class UEnemySpawnParamaters;
class AEnemyPawn;

UCLASS()
class GRIEF_API AEnemySpawner : public AActor, public IPlatformActorInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PreviewBox = nullptr;

public:
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UFUNCTION()
	void Spawn();

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = "true"))
	class AEnemySpawnTrigger* SpawnTrigger = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyPawn> EnemyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = "true"))
	bool SpawnOnBeginPlay = true;
	
	UPROPERTY(EditAnywhere, Instanced, Category = Spawning, DisplayName = "Spawn Paramaters", meta = (AllowPrivateAccess = "true"))
	UEnemySpawnParamaters* EnemySpawnParamaters = nullptr;

private:
	UPROPERTY()
	AEnemyPawn* EnemyPawn = nullptr;

	FTransform SpawnTransform;

public:
	virtual void ResetPlatformActor() override;
};
