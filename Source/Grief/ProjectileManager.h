// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleProjectile.h"
#include "ProjectileManager.generated.h"


UCLASS()
class GRIEF_API AProjectileManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	AProjectileManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintPure)
	ASimpleProjectile* GetProjectile();

private:
	ASimpleProjectile* SpawnProjectile(UWorld* World);

public:
	void RetrieveProjectile(ASimpleProjectile* Projectile);

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Pool, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASimpleProjectile> ProjectileClass;

private:
	UPROPERTY()
	TArray<ASimpleProjectile*> ProjectilePool;
};
