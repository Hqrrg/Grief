// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "PlatformCharacterMovementComponent.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyCharacter::UpdateMoving()
{
	Moving = GetMovementVector().Length() > 0.0f;
	UpdateFlipbook();
}

FVector2D AEnemyCharacter::GetMovementVector()
{
	FVector2D MovementVector = FVector2D::ZeroVector;
	
	FVector AccelerationVector = PlatformCharacterMovementComponent->GetCurrentAcceleration();
	float Acceleration = AccelerationVector.Length();

	if (AccelerationVector.Normalize())
	{
		float MaxAcceleration = PlatformCharacterMovementComponent->GetMaxAcceleration();
		float X = Acceleration / MaxAcceleration;

		FVector Input = AccelerationVector * X;
		
		const FVector ForwardVector = GetActorForwardVector();
		const FVector RightVector = GetActorRightVector();

		const float MovementVectorX = FVector::DotProduct(Input, RightVector);
		const float MovementVectorY = FVector::DotProduct(Input, ForwardVector);

		MovementVector = FVector2D(MovementVectorX, MovementVectorY);
	}
	
	return MovementVector;
}

UBehaviorTree* AEnemyCharacter::GetBehaviourTree()
{
	return BehaviourTree;
}

bool AEnemyCharacter::Attack(uint8 AttackID)
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

void AEnemyCharacter::Killed()
{
	Destroy();
}

