// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "PlatformCameraComponent.h"
#include "PlatformCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AttackHitboxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Enums/Direction.h"
#include "Interfaces/EnemyInterface.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformCharacterMovementComponent>(CharacterMovementComponentName))
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	HighAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("HighAttackHitbox"));
	HighAttackHitbox->SetupAttachment(FlipbookComponent);

	MiddleAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("MiddleAttackHitbox"));
	MiddleAttackHitbox->SetupAttachment(FlipbookComponent);
	
	LowAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("LowAttackHitbox"));
	LowAttackHitbox->SetupAttachment(FlipbookComponent);
	
	PlatformCameraComponent = CreateDefaultSubobject<UPlatformCameraComponent>(TEXT("PlatformCameraComponent"));
	AddOwnedComponent(PlatformCameraComponent);
	
	JumpMaxCount = 2;
}

void APlayerCharacter::BeginPlay()
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
	if (Attacking || AttackInfoArray.IsEmpty()) return;

	uint8 AttackID = 0;

	if (AttackID < AttackInfoArray.Num())
	{
		const FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
		
		if (AttackInfo->IsCooldown) return;
		
		Attacking = true;

		AttackingFlipbook = AttackInfo->Flipbook;
		UpdateFlipbook();

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

		if (Hitbox)
		{
			TArray<AActor*> OverlappingCombatants = Hitbox->GetOverlappingCombatants();
		
			if (OverlappingCombatants.Num() > 0)
			{
				for (uint8 Index = 0; Index < OverlappingCombatants.Num(); Index++)
				{
					AActor* CurrentCombatant = OverlappingCombatants[Index];
					
					if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(CurrentCombatant))
					{
						ICombatantInterface* Combatant = Cast<ICombatantInterface>(CurrentCombatant);
						
						if (Combatant->IsObscured(this) || !Combatant->IsAlive()) continue;
						
						Combatant->ApplyDamage(AttackInfo->Damage);
						Combatant->Knockback(GetActorLocation(), AttackInfo->KnockbackMultiplier);
					}
				}
			}
		}
	
		float AttackDuration = 1.0f;
		if (AttackingFlipbook) AttackDuration = AttackingFlipbook->GetTotalDuration();
	
		FTimerHandle AttackTimerHandle;
		FTimerDelegate AttackTimerDelegate;
		
		AttackTimerDelegate.BindUFunction(this, FName("StopAttacking"), AttackID);
		
		GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, AttackDuration, false, AttackDuration);
	}
}
