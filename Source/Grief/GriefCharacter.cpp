// Fill out your copyright notice in the Description page of Project Settings.


#include "GriefCharacter.h"

#include "PlatformCharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Enums/Direction.h"

AGriefCharacter::AGriefCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformCharacterMovementComponent>(CharacterMovementComponentName))
{
	FlipbookComponent = GetSprite();
	FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
}

void AGriefCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlatformCharacterMovementComponent = Cast<UPlatformCharacterMovementComponent>(GetMovementComponent());
	
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

void AGriefCharacter::SetPlatformerMovementMode(const EPlatformerMovementMode InPlatformerMovementMode)
{
	PlatformerMovementMode = InPlatformerMovementMode;
	UpdateFlipbook();
}

void AGriefCharacter::UpdateFlipbook()
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

	switch (PlatformerMovementMode)
	{
	case EPlatformerMovementMode::Grounded:
		break;
		
	case EPlatformerMovementMode::Jumping:
		Flipbook = Flipbooks->Jumping;
		break;
		
	case EPlatformerMovementMode::Falling:
		Flipbook = Flipbooks->Falling;
		break;
	}

	if (IsAttacking())
	{
		Flipbook = AttackingFlipbook;
	}

	if (FlipbookComponent->GetFlipbook() != Flipbook) FlipbookComponent->SetFlipbook(Flipbook);
}

float AGriefCharacter::GetMaxHealth()
{
	return MaxHealth;
}

float AGriefCharacter::GetHealth()
{
	return Health;
}

bool AGriefCharacter::IsObscured(const AActor* TargetActor)
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

void AGriefCharacter::Knockback(const FVector OriginLocation, const float KnockbackMultiplier)
{
	if (!ShouldKnockback()) return;
	
	const FVector ActorLocation = GetActorLocation();
	FVector KnockbackDirection = (ActorLocation - OriginLocation).GetSafeNormal();
	FVector FinalKnockbackDirection = FVector(0.0f, KnockbackDirection.Y < 0.0f ? -1.0f : 1.0f, 0.0f);
	
	const float TotalKnockback = GetKnockbackAmount() * KnockbackMultiplier;
	
	PlatformCharacterMovementComponent->Knockback(FinalKnockbackDirection, TotalKnockback);
}

float AGriefCharacter::GetKnockbackAmount()
{
	return KnockbackAmount;
}

void AGriefCharacter::SetMaxHealth(const float InMaxHealth)
{
	MaxHealth = InMaxHealth;
}

void AGriefCharacter::SetHealth(const float InHealth)
{
	Health = InHealth;
}

void AGriefCharacter::StopAttacking(uint8 AttackID)
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

void AGriefCharacter::RemoveAttackCooldown(uint8 AttackID)
{
	if (AttackID < AttackInfoArray.Num())
	{
		FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
		AttackInfo->IsCooldown = false;
	}
}

void AGriefCharacter::UpdateDirections(const FVector2D MovementVector)
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
