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
	
	AttackDirection = MovementDirection;

	if (FlipbookDataTable)
	{
		static const FString ContextString(FString::Printf(TEXT("%s Character Flipbooks Context"), *FlipbookKey.ToString()));
		Flipbooks = FlipbookDataTable->FindRow<FCharacterFlipbooks>(FlipbookKey, ContextString, true);
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
		Flipbook = Flipbooks->Attacking;
	}

	if (FlipbookComponent->GetFlipbook() != Flipbook) FlipbookComponent->SetFlipbook(Flipbook);
}

void AGriefCharacter::StopAttacking()
{
	Attacking = false;
	UpdateFlipbook();
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
