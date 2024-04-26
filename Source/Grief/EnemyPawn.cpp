// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"

#include "PaperFlipbook.h"


// Sets default values
AEnemyPawn::AEnemyPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyPawn::UpdateMoving()
{
	Moving = GetMovementVector().Length() > 0.0f && GetVelocity().Length() != 0.0f;
	UpdateFlipbook();
}

FVector2D AEnemyPawn::GetMovementVector()
{
	FVector LastInputVector = GetPlatformMovementComponent()->GetLastInputVector();
	FVector2D MovementVector = FVector2D(LastInputVector.Y, LastInputVector.X);

	return MovementVector;
}

UBehaviorTree* AEnemyPawn::GetBehaviourTree()
{
	return BehaviourTree;
}

bool AEnemyPawn::Attack(uint8 AttackID)
{
	if (Attacking || AttackInfoArray.IsEmpty()) return false;

	if (AttackID > AttackInfoArray.Num()-1) return false;
	
	const FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
	
	if (AttackInfo->IsCooldown) return false;
	
	Attacking = true;
	
	AttackingFlipbook = AttackInfo->Flipbook;
	UpdateFlipbook();

	float AttackDuration = 1.0f;
	if (AttackingFlipbook) AttackDuration = AttackingFlipbook->GetTotalDuration();

	FTimerHandle AttackTimerHandle;
	FTimerDelegate AttackTimerDelegate;

	AttackTimerDelegate.BindUFunction(this, FName("StopAttacking"), AttackID);
	
	GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, AttackDuration, false, AttackDuration);

	return true;
}

void AEnemyPawn::Killed()
{
	Destroy();
}

