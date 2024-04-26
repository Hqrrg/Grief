// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"

#include "PaperFlipbookComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbook.h"
#include "Enums/Direction.h"

ABasePawn::ABasePawn()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetBoxExtent(FVector(35.0f, 35.0f, 90.0f));
	CollisionComponent->ShapeColor = FColor::White;
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;
	SetRootComponent(CollisionComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->ArrowColor = FColor::Cyan;
	ArrowComponent->SetupAttachment(CollisionComponent);
	
	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	FlipbookComponent->SetupAttachment(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UPlatformPawnMovement>("MovementComponent");
	MovementComponent->SetUpdatedComponent(CollisionComponent);
	AddOwnedComponent(MovementComponent);
}

void ABasePawn::BeginPlay()
{
	Super::BeginPlay();

	GetPlatformMovementComponent()->MovementModeUpdated.AddDynamic(this, &ABasePawn::UpdateFlipbook);
	
	AttackDirection = MovementDirection;

	if (FlipbookDataTable)
	{
		static const FString ContextString(FString::Printf(TEXT("%s Character Flipbooks Context"), *FlipbookKey.ToString()));
		Flipbooks = FlipbookDataTable->FindRow<FCharacterFlipbooks>(FlipbookKey, ContextString, true);
	}

	if (AttacksDataTable)
	{
		for (FName RowName : AttacksDataTable->GetRowNames())
		{
			static const FString ContextString(FString::Printf(TEXT("%s : Attack Info Context"), *RowName.ToString()));
			FAttackInfo AttackInfo = *AttacksDataTable->FindRow<FAttackInfo>(RowName, ContextString, true);
			
			AttackInfoArray.Add(AttackInfo);
		}
	}
}

void ABasePawn::UpdateFlipbook()
{
	if (!Flipbooks) return;

	UPaperFlipbook* Flipbook = Flipbooks->Idling;
	
	switch (MovementDirection)
	{
	case EDirection::Left:
		FlipbookComponent->SetRelativeRotation(FRotator(0.0f , -90.0f, 0.0f));
		if (IsMoving()) Flipbook = Flipbooks->Walking;
		break;
		
	case EDirection::Right:
		FlipbookComponent->SetRelativeRotation(FRotator(0.0f , 90.0f, 0.0f));
		if (IsMoving()) Flipbook = Flipbooks->Walking;
		break;
		
	default:
		break;
	}

	switch (GetPlatformMovementComponent()->GetMovementMode())
	{
	case EPlatformMovementMode::Walking:
		break;
		
	case EPlatformMovementMode::Jumping:
		Flipbook = Flipbooks->Jumping;
		break;
		
	case EPlatformMovementMode::Falling:
		Flipbook = Flipbooks->Falling;
		break;
	}

	if (IsAttacking())
	{
		Flipbook = AttackingFlipbook;
	}

	if (FlipbookComponent->GetFlipbook() != Flipbook) FlipbookComponent->SetFlipbook(Flipbook);
}

float ABasePawn::GetMaxHealth()
{
	return MaxHealth;
}

float ABasePawn::GetHealth()
{
	return Health;
}

bool ABasePawn::IsObscured(const AActor* TargetActor)
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

void ABasePawn::Knockback(const FVector OriginLocation, const float KnockbackMultiplier)
{
	if (!ShouldKnockback()) return;
	
	const FVector ActorLocation = GetActorLocation();
	FVector KnockbackDirection = (ActorLocation - OriginLocation).GetSafeNormal();
	FVector FinalKnockbackDirection = FVector(0.0f, KnockbackDirection.Y < 0.0f ? -1.0f : 1.0f, 0.0f);
	
	const float TotalKnockback = GetKnockbackAmount() * KnockbackMultiplier;
	
	GetPlatformMovementComponent()->Knockback(FinalKnockbackDirection, TotalKnockback);
}

float ABasePawn::GetKnockbackAmount()
{
	return KnockbackAmount;
}

void ABasePawn::SetMaxHealth(const float InMaxHealth)
{
	MaxHealth = InMaxHealth;
}

void ABasePawn::SetHealth(const float InHealth)
{
	Health = InHealth;
}

void ABasePawn::StopAttacking(uint8 AttackID)
{
	Attacking = false;
	UpdateFlipbook();

	if (AttackID < AttackInfoArray.Num())
	{
		FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
		float AttackCooldownDuration = AttackInfo->Cooldown;
		
		if (AttackCooldownDuration > 0.0f)
		{
			AttackInfo->IsCooldown = true;
			
			FTimerHandle AttackCooldownHandle;
			FTimerDelegate AttackCooldownDelegate;
		
			AttackCooldownDelegate.BindUFunction(this, FName("RemoveAttackCooldown"), AttackID);

			GetWorldTimerManager().SetTimer(AttackCooldownHandle, AttackCooldownDelegate, AttackCooldownDuration, false, AttackCooldownDuration);
		}
	}
}

void ABasePawn::RemoveAttackCooldown(uint8 AttackID)
{
	if (AttackID < AttackInfoArray.Num())
	{
		FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
		AttackInfo->IsCooldown = false;
	}
}

void ABasePawn::UpdateDirections(const FVector2D MovementVector)
{
	/* Reset Attack Direction */
	AttackDirection = EDirection::None;
		
	/* Up */
	if (MovementVector.Y > 0.0f)
	{
		AttackDirection |= EDirection::Up;
	}
	/* Down */
	else if (MovementVector.Y < 0.0f)
	{
		AttackDirection |= EDirection::Down;
	}
	/* Right */
	if (MovementVector.X > 0.0f)
	{
		AttackDirection |= EDirection::Right;
		MovementDirection = EDirection::Right;
	}
	/* Left */
	else if (MovementVector.X < 0.0f)
	{
		AttackDirection |= EDirection::Left;
		MovementDirection = EDirection::Left;
	}

	/* Don't allow for straight up or down attacks */
	if (AttackDirection == EDirection::None || AttackDirection == EDirection::Up || AttackDirection == EDirection::Down)
	{
		AttackDirection = MovementDirection;
	}
}
