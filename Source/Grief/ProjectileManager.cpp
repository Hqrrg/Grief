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
	if (ProjectilePool.IsEmpty())
	{
		UWorld* World = GetWorld();
		return SpawnProjectile(World);
	}

	const int32 LastIndex = ProjectilePool.Num()-1;
	
	ASimpleProjectile* Projectile = ProjectilePool[LastIndex];
	
	ProjectilePool.RemoveAt(LastIndex);
	
	return Projectile;
}

ASimpleProjectile* AProjectileManager::SpawnProjectile(UWorld* World)
{
	if (!World) return nullptr;
	if (!ProjectileClass) return nullptr;
	
	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
	
	ASimpleProjectile* Projectile = World->SpawnActorDeferred<ASimpleProjectile>(ProjectileClass, SpawnTransform);
	
	if (!Projectile) return nullptr;
	
	Projectile->FinishSpawning(SpawnTransform);
	Projectile->SetProjectileManager(this);
	ProjectilePool.Add(Projectile);

	return Projectile;
}

void AProjectileManager::RetrieveProjectile(ASimpleProjectile* Projectile)
{
	if (ProjectilePool.Num() < 20)
	{
		const FVector Location = FVector::ZeroVector;
		const FRotator Rotation = FRotator::ZeroRotator;
		
		Projectile->TeleportTo(Location, Rotation);
		
		if (ProjectilePool.Contains(Projectile)) return;
		
		ProjectilePool.Add(Projectile);
		return;
	}
	
	Projectile->Destroy();
}

