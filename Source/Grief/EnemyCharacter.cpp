// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "HitboxComponent.h"
#include "PlatformCharacterMovementComponent.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackHitbox = CreateDefaultSubobject<UHitboxComponent>(TEXT("AttackHitbox"));
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyCharacter::UpdateMoving()
{
	Moving = GetMovementVector().Length() > 0.0f;
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

		const float MovementVectorX = FVector::DotProduct(Input, ForwardVector);
		const float MovementVectorY = FVector::DotProduct(Input, RightVector);

		MovementVector = FVector2D(MovementVectorX, MovementVectorY);
	}
	
	return MovementVector;
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

void AEnemyCharacter::Knockback(const FVector OriginLocation, const float KnockbackMultiplier)
{
	if (!ShouldKnockback()) return;
	
	const FVector ActorLocation = GetActorLocation();
	FVector KnockbackDirection = (ActorLocation - OriginLocation).GetSafeNormal();
	FVector FinalKnockbackDirection = FVector(0.0f, KnockbackDirection.Y < 0.0f ? -1.0f : 1.0f, 0.0f);
	
	const float TotalKnockback = GetKnockbackAmount() * KnockbackMultiplier;
	
	PlatformCharacterMovementComponent->Knockback(FinalKnockbackDirection, TotalKnockback);
}

void AEnemyCharacter::Attack(uint8 AttackID)
{
}

float AEnemyCharacter::GetKnockbackAmount()
{
	return KnockbackAmount;
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

