// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "PlatformCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AttackHitboxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Enums/Direction.h"
#include "Interfaces/EnemyInterface.h"

APlayerPawn::APlayerPawn()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	HighAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("HighAttackHitbox"));
	HighAttackHitbox->SetupAttachment(GetFlipbookComponent());

	MiddleAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("MiddleAttackHitbox"));
	MiddleAttackHitbox->SetupAttachment(GetFlipbookComponent());
	
	LowAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("LowAttackHitbox"));
	LowAttackHitbox->SetupAttachment(GetFlipbookComponent());
	
	PlatformCameraComponent = CreateDefaultSubobject<UPlatformCameraComponent>(TEXT("PlatformCameraComponent"));
	AddOwnedComponent(PlatformCameraComponent);

	MaxHealth = 6.0f;
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	/* Add input mapping context */
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/* Movement */
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerPawn::Move);

		/* Jumping */
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerPawn::Jump);

		/* Attacking */
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerPawn::Attack);
	}
}

bool APlayerPawn::Killed()
{
	bool IsAnimationFinished = Super::Killed();

	if (IsAnimationFinished)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString("Killed"));
	}
	
	return IsAnimationFinished;
}

void APlayerPawn::Move(const FInputActionValue& Value)
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

void APlayerPawn::Jump(const FInputActionValue& Value)
{
	bool JumpKeyPressed = Value.Get<bool>();

	if (JumpKeyPressed)
	{
		GetPlatformMovementComponent()->Jump();
	}
	else
	{
		GetPlatformMovementComponent()->StopJumping();
	}
}

void APlayerPawn::Attack(const FInputActionValue& Value)
{
	if (Attacking || AttackInfoArray.IsEmpty()) return;

	constexpr uint8 AttackID = static_cast<uint8>(EPlayerAttack::Default);

	if (AttackID < AttackInfoArray.Num())
	{
		const FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
		
		if (AttackInfo->IsCooldown) return;

		UAttackHitboxComponent* Hitbox = nullptr;

		switch (AttackDirection)
		{
		case EDirection::UpRight:
			Hitbox = HighAttackHitbox;
			break;
		case EDirection::UpLeft:
			Hitbox = HighAttackHitbox;
			break;
		case EDirection::Right:
			Hitbox = MiddleAttackHitbox;
			break;
		case EDirection::Left:
			Hitbox = MiddleAttackHitbox;
			break;
		case EDirection::DownRight:
			Hitbox = LowAttackHitbox;
			break;
		case EDirection::DownLeft:
			Hitbox = LowAttackHitbox;
			break;
		default:
			break;
		}

		if (!Hitbox) return;

		Attacking = true;

		AttackingFlipbook = AttackInfo->Flipbook;
		UpdateFlipbook();
	
		float AttackDuration = 1.0f;
		if (AttackingFlipbook) AttackDuration = AttackingFlipbook->GetTotalDuration();
	
		FTimerHandle AttackTimerHandle;
		FTimerDelegate AttackTimerDelegate;
		
		AttackTimerDelegate.BindUFunction(this, FName("StopAttacking"), AttackID);
		GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, AttackDuration, false, AttackDuration);

		DefaultAttackTimerDelegate.BindUFunction(this, FName("DefaultAttack"), Hitbox);
		DefaultAttack(Hitbox);

		OnBeginAttack(AttackID);
	}
}

void APlayerPawn::DefaultAttack(UAttackHitboxComponent* Hitbox)
{
	float PlaybackBegin, PlaybackEnd;

	const uint8 AttackID = GetAttackID(EPlayerAttack::Default);
	const FAttackInfo* DefaultAttackInfo = &AttackInfoArray[AttackID];

	if (!DoAttack(AttackID, DefaultAttackTimerHandle,DefaultAttackTimerDelegate, DefaultAttackInfo->BeginFrame, DefaultAttackInfo->EndFrame,PlaybackBegin, PlaybackEnd)) return;

	if (DefaultAttackTriggered) return;
	
	TArray<AActor*> ContainedActors = Hitbox->GetContainedActors();
	
	if (ContainedActors.Num() > 0)
	{
		for (uint8 Index = 0; Index < ContainedActors.Num(); Index++)
		{
			AActor* CurrentCombatant = ContainedActors[Index];
				
			if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(CurrentCombatant))
			{
				ICombatantInterface* Combatant = Enemy->GetCombatant();
					
				if (Combatant->IsObscured(this) || !Combatant->IsAlive()) continue;

				Combatant->Knockback(GetActorLocation(), DefaultAttackInfo->KnockbackMultiplier);
				Combatant->ApplyDamage(DefaultAttackInfo->Damage);
			}
		}
	}
	
	DefaultAttackTriggered = true;
	OnAttack(GetAttackID(EPlayerAttack::Default));
}

void APlayerPawn::OnAttackFinished(uint8 AttackID)
{
	constexpr uint8 DefaultAttackID = static_cast<uint8>(EPlayerAttack::Default);

	switch (AttackID)
	{
	case DefaultAttackID:
		DefaultAttackTriggered = false;
		break;
		
	default:
		break;
	}
}

ICombatantInterface* APlayerPawn::GetCombatant()
{
	return this;
}

bool APlayerPawn::IsInvincible()
{
	return Invincible;
}

void APlayerPawn::ApplyDamage(const float Damage)
{
	Super::ApplyDamage(Damage);

	if (!IsInvincible())
	{
		Invincible = true;

		FTimerHandle InvincibilityTimerHandle;
		GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &APlayerPawn::RemoveInvincibility, InvincibilityDuration, false, InvincibilityDuration);
	}
}

void APlayerPawn::RemoveInvincibility()
{
	Invincible = false;
}
