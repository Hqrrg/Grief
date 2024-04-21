// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

float AEnemyCharacter::GetMaxHealth()
{
	return MaxHealth;
}

float AEnemyCharacter::GetHealth()
{
	return Health;
}

UBehaviorTree* AEnemyCharacter::GetBehaviourTree()
{
	return BehaviourTree;
}

bool AEnemyCharacter::IsObscured(const AActor* TargetActor)
{
	bool Obscured = false;

	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TargetActor->GetActorLocation();

	const bool IsBlockingHit = GetWorld()->LineTraceSingleByChannel(
		*HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		CollisionQueryParams);

	if (IsBlockingHit)
	{
		Obscured = HitResult->GetActor() != TargetActor;
	}

	return Obscured;
}

void AEnemyCharacter::SetMaxHealth(const float InMaxHealth)
{
	MaxHealth = InMaxHealth;
}

void AEnemyCharacter::SetHealth(const float InHealth)
{
	Health = InHealth;
}

void AEnemyCharacter::Killed()
{
	Destroy();
}

