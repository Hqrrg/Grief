// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PlayerSensingComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AEnemyPawn::AEnemyPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerSensing = CreateDefaultSubobject<UPlayerSensingComponent>(TEXT("PlayerSensingComponent"));
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

ICombatantInterface* AEnemyPawn::GetCombatant()
{
	return this;
}

bool AEnemyPawn::DoAttack(FTimerHandle& TimerHandle, FTimerDelegate& Callback, uint8 BeginFrame, uint8 EndFrame, float& PlaybackBegin, float& PlaybackEnd)
{
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorldTimerManager().SetTimer(TimerHandle, Callback, 0.01f, true);
		return false;
	}
	
	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax)
	{
		if (GetWorldTimerManager().IsTimerActive(TimerHandle)) GetWorldTimerManager().ClearTimer(TimerHandle);
		return false;
	}
	
	float Framerate = GetFlipbookComponent()->GetFlipbookFramerate();

	PlaybackBegin = BeginFrame / Framerate;
	PlaybackEnd = EndFrame / Framerate;

	if (PlaybackCurrent < PlaybackBegin || PlaybackCurrent > PlaybackEnd) return false;

	return true;
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

