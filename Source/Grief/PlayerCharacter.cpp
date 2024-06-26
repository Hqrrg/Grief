// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "PlatformCameraComponent.h"
#include "PlatformCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HitboxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Enums/Direction.h"
#include "Interfaces/EnemyInterface.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformCharacterMovementComponent>(CharacterMovementComponentName))
{
	FlipbookComponent = GetSprite();
	FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	PlatformCameraComponent = CreateDefaultSubobject<UPlatformCameraComponent>(TEXT("PlatformCameraComponent"));
	AddOwnedComponent(PlatformCameraComponent);
	
	LowHitbox = CreateDefaultSubobject<UHitboxComponent>(TEXT("LowHitbox"));
	LowHitbox->SetupAttachment(FlipbookComponent);

	MiddleHitbox = CreateDefaultSubobject<UHitboxComponent>(TEXT("MiddleHitbox"));
	MiddleHitbox->SetupAttachment(FlipbookComponent);
	
	HighHitbox = CreateDefaultSubobject<UHitboxComponent>(TEXT("HighHitbox"));
	HighHitbox->SetupAttachment(FlipbookComponent);
	
	JumpMaxCount = 2;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementDirection = EDirection::Right;
	AttackDirection = EDirection::Right;
	
	/* Add input mapping context */
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (FlipbookDataTable)
	{
		static const FString ContextString(TEXT("Grief Character Flipbooks Context"));
		Flipbooks = FlipbookDataTable->FindRow<FCharacterFlipbooks>(FName("Default"), ContextString, true);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/* Movement */
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		/* Jumping */
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APaperCharacter::Jump);

		/* Attacking */
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (!Attacking)
		{
			AddMovementInput(RightDirection, MovementVector.X);
			UpdateDirections(MovementVector);
		}
		
		Moving = MovementVector != FVector2D::ZeroVector;
		UpdateFlipbook();
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (Attacking) return;
	
	Attacking = true;
	
	UpdateFlipbook();

	UHitboxComponent* Hitbox = nullptr;

	switch (AttackDirection)
	{
		case EDirection::UpRight:
			Hitbox = HighHitbox;
			break;
		case EDirection::UpLeft:
			Hitbox = HighHitbox;
			break;
		case EDirection::Right:
			Hitbox = MiddleHitbox;
			break;
		case EDirection::Left:
			Hitbox = MiddleHitbox;
			break;
		case EDirection::DownRight:
			Hitbox = LowHitbox;
			break;
		case EDirection::DownLeft:
			Hitbox = LowHitbox;
			break;
		default:
			break;
	}

	if (Hitbox)
	{
		TArray<AActor*> OverlappingEnemies = Hitbox->GetOverlappingEnemies();
		
		if (OverlappingEnemies.Num() > 0)
		{
			for (uint8 Index = 0; Index < OverlappingEnemies.Num(); Index++)
			{
				IEnemyInterface* Enemy = Cast<IEnemyInterface>(OverlappingEnemies[Index]);

				if (Enemy && (Enemy->IsObscured(this) || !Enemy->IsAlive())) continue;
				
				Enemy->ApplyDamage(MeleeDamage);
			}
		}
	}
	
	float AttackDuration = 1.0f;
	if (Flipbooks) AttackDuration = Flipbooks->Attacking->GetTotalDuration();
	
	FTimerHandle AttackTimerHandle;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlayerCharacter::StopAttacking, AttackDuration, false, AttackDuration);
}

void APlayerCharacter::SetPlatformerMovementMode(const EPlatformerMovementMode InPlatformerMovementMode)
{
	PlatformerMovementMode = InPlatformerMovementMode;
	UpdateFlipbook();
}

void APlayerCharacter::UpdateDirections(const FVector2D MovementVector)
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

void APlayerCharacter::UpdateFlipbook()
{
	if (!Flipbooks) return;

	UPaperFlipbook* Flipbook = Flipbooks->Idling;

	if (IsMoving())
	{
		Flipbook = Flipbooks->Walking;
	}
	
	switch (MovementDirection)
	{
	case EDirection::Left:
		FlipbookComponent->SetRelativeRotation(FRotator(0.0f , -90.0f, 0.0f));
		break;
		
	case EDirection::Right:
		FlipbookComponent->SetRelativeRotation(FRotator(0.0f , 90.0f, 0.0f));
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

void APlayerCharacter::StopAttacking()
{
	Attacking = false;
	
	UpdateFlipbook();
}
