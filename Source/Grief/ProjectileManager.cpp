// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileManager.h"


// Sets default values for this component's properties
AProjectileManager::AProjectileManager()
{
	PrimaryActorTick.bCanEverTick = false;
}


// Called when the game starts
void AProjectileManager::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectilePool = TArray<ASimpleProjectile*, TFixedAllocator<20>>();

	UWorld* World = GetWorld();

	if (!World) return;

	for (int32 i = 0; i < 20; i++)
	{
		SpawnProjectile(World);
	}
}

ASimpleProjectile* AProjectileManager::GetProjectile()
{
	UWorld* World = GetWorld();

	// Pool empty, spawn new projectile
	if (ProjectilePool.IsEmpty()) return SpawnProjectile(World);

	// Get last projectile in pool
	const int32 LastIndex = ProjectilePool.Num()-1;
	ASimpleProjectile* Projectile = ProjectilePool[LastIndex];

	if (!Projectile) Projectile = SpawnProjectile(World);
	
	//Remove projectile from pool before returning
	ProjectilePool.RemoveAt(LastIndex);
	return Projectile;
}

ASimpleProjectile* AProjectileManager::SpawnProjectile(UWorld* World)
{
	// Don't spawn if world is invalid or there is no projectile class assigned
	if (!World) return nullptr;
	if (!ProjectileClass) return nullptr;
	
	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
	
	ASimpleProjectile* Projectile = World->SpawnActorDeferred<ASimpleProjectile>(ProjectileClass, SpawnTransform);
	
	if (!Projectile) return nullptr;
	// Set projectile manager to this and add to pool
	Projectile->FinishSpawning(SpawnTransform);
	Projectile->SetProjectileManager(this);
	ProjectilePool.Add(Projectile);

	return Projectile;
}

void AProjectileManager::RetrieveProjectile(ASimpleProjectile* Projectile)
{
	if (ProjectilePool.Num() < 20) // If there is space in the pool
	{
		const FVector Location = FVector::ZeroVector;
		const FRotator Rotation = FRotator::ZeroRotator;
		
		Projectile->TeleportTo(Location, Rotation); // Reset location
		// Already contains this projectile
		if (ProjectilePool.Contains(Projectile)) return;
		// Add to pool
		ProjectilePool.Add(Projectile);
		return;
	}
	// Else destroy 
	Projectile->Destroy();
}

