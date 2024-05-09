// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnemySpawnParamaters.generated.h"

class AAIPatrolRoute;
class AMovementBoundingBox;
class AProjectileManager;
class AEnemySpawner;

enum class EButterflyPathType : uint8;

// Enemy
UCLASS(DefaultToInstanced, EditInlineNew, meta = (UsesHierarchy))
class GRIEF_API UEnemySpawnParamaters : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly,  Category = "Enemy")
	AAIPatrolRoute* PatrolRoute = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Enemy")
	AMovementBoundingBox* MovementBoundingBox = nullptr;
};

// Denial
UCLASS()
class GRIEF_API UDenialSpawnParamaters : public UEnemySpawnParamaters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Denial")
	AProjectileManager* LaserProjectileManager = nullptr;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly,  Category = "Denial")
	AEnemySpawner* AngerSpawner = nullptr;
};

// Anger
UCLASS()
class GRIEF_API UAngerSpawnParamaters : public UEnemySpawnParamaters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Anger")
	AProjectileManager* FireballProjectileManager = nullptr;
};

// Butterfly
UCLASS()
class GRIEF_API UButterflySpawnParamaters : public UEnemySpawnParamaters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Butterfly")
	EButterflyPathType ButterflyPathType;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Butterfly")
	float MovementPathRadius = 100.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Butterfly")
	float Speed = 3.0f;
};
