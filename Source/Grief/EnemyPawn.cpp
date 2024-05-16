// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"

#include "EnemyAIController.h"
#include "EnemySpawnParamaters.h"
#include "MovementBoundingBox.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PlayerSensingComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AEnemyPawn::AEnemyPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PlayerSensing = CreateDefaultSubobject<UPlayerSensingComponent>(TEXT("PlayerSensing"));
	AddOwnedComponent(PlayerSensing);
	
	FlipbookComponent->SetRelativeLocation(FVector(-1.0f, 0.0f, 0.0f));
	CollisionComponent->SetCollisionProfileName(FName("Enemy"));
	
	bUseControllerRotationYaw = false;

	MaxHealth = 2.0f;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnParamaters)
	{
		PatrolRoute = SpawnParamaters->PatrolRoute;
		MovementBoundingBox = SpawnParamaters->MovementBoundingBox;
	}
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

void AEnemyPawn::StopAttacking(uint8 AttackID)
{
	Super::StopAttacking(AttackID);

	if (Controller)
	{
		Controller->SetIgnoreMoveInput(false);
	}
}

UBehaviorTree* AEnemyPawn::GetBehaviourTree()
{
	return BehaviourTree;
}

bool AEnemyPawn::Attack(uint8 AttackID, bool StopMovement)
{
	if (IsDying() || Attacking || AttackInfoArray.IsEmpty()) return false;

	if (AttackID > AttackInfoArray.Num()-1) return false;
	
	const FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
	
	if (AttackInfo->IsCooldown) return false;
	
	Attacking = true;

	if (Controller)
	{
		Controller->SetIgnoreMoveInput(StopMovement);
	}
	
	AttackingFlipbook = AttackInfo->Flipbook;
	UpdateFlipbook();

	float AttackDuration = 1.0f;
	if (AttackingFlipbook) AttackDuration = AttackingFlipbook->GetTotalDuration();

	FTimerHandle AttackTimerHandle;
	FTimerDelegate AttackTimerDelegate;

	AttackTimerDelegate.BindUFunction(this, FName("StopAttacking"), AttackID);
	
	GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, AttackDuration, false, AttackDuration);

	OnBeginAttack(AttackID);
	return true;
}

ICombatantInterface* AEnemyPawn::GetCombatant()
{
	return this;
}

void AEnemyPawn::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (!MovementBoundingBox)
	{
		Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
		return;
	}

	FVector FinalWorldDirection = FVector::ZeroVector;

	for (int32 C = 0; C < 3; C++)
	{
		FVector Temp = FVector::ZeroVector; Temp.Component(C) = WorldDirection.Component(C);
		float Direction = Temp.Component(C) > 0.0f ? 1.0f : -1.0f;
		FVector Buffer = GetActorLocation(); Buffer.Component(C)+=(GetCollisionComponent()->GetScaledBoxExtent().Component(C)+10.0f)*Direction;
		FVector TempTargetLocation = Buffer + Temp;
		
		if (!MovementBoundingBox->IsLocationWithinArea(TempTargetLocation)) continue;

		FinalWorldDirection.Component(C) = WorldDirection.Component(C);
	}

	FVector ActorLocation = GetActorLocation();
	FVector CollisionBoxExtent = GetCollisionComponent()->GetScaledBoxExtent();
	FVector Direction = MovementBoundingBox->GetActorLocation() - ActorLocation;
	float DirectionY = Direction.Y > 0.0f ? 1.0f : -1.0f;
	float DirectionZ = Direction.Z > 0.0f ? 1.0f : -1.0f;

	FVector TargetLocation = FVector(ActorLocation.X, ActorLocation.Y-CollisionBoxExtent.Y*DirectionY, ActorLocation.Z-CollisionBoxExtent.Z*DirectionZ);
	
	if (!MovementBoundingBox->IsLocationWithinArea(TargetLocation))
	{
		FVector DirectionNormalized = Direction.GetSafeNormal();
		float MovementDirectionY = DirectionNormalized.Y;
		float MovementDirectionZ = DirectionNormalized.Z;
		
		FinalWorldDirection = FVector(0.0f, MovementDirectionY, 0.0f);
		
		if (MovementComponent->GetMovementMode() == EPlatformMovementMode::Flying)
		{
			FinalWorldDirection = FVector(0.0f, MovementDirectionY, MovementDirectionZ);
		}
	}
	
	Super::AddMovementInput(FinalWorldDirection, ScaleValue, bForce);
}

void AEnemyPawn::ResetPlatformActor()
{
	Super::ResetPlatformActor();
	
	Destroy();
}

bool AEnemyPawn::Killed()
{
	bool IsAnimationFinished = Super::Killed();

	if (IsAnimationFinished)
	{
		// Add timer to fade out before destroy
		Destroy();
		return true;
	}
	
	return false;
}

