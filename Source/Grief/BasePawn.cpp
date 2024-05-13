// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"

#include "PaperFlipbookComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Enums/Direction.h"

ABasePawn::ABasePawn()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetBoxExtent(FVector(35.0f, 35.0f, 90.0f));
	CollisionComponent->ShapeColor = FColor::White;
	CollisionComponent->SetCollisionProfileName(FName("Pawn"));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	SetRootComponent(CollisionComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->ArrowColor = FColor::Cyan;
	ArrowComponent->SetupAttachment(CollisionComponent);
	
	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetupAttachment(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UPlatformPawnMovement>("MovementComponent");
	MovementComponent->SetUpdatedComponent(CollisionComponent);
	MovementComponent->bConstrainToPlane = true;
	MovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
	AddOwnedComponent(MovementComponent);
}

void ABasePawn::BeginPlay()
{
	Super::BeginPlay();

	SetHealth(MaxHealth);

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

	UpdateFlipbook();
}

void ABasePawn::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);

	if (MovementComponent->GetMovementMode() == EPlatformMovementMode::Walking)
	{
		if (Flipbooks && !IsAttacking() && IsMoving())
		{
			TArray<float>* FootstepFrames = &Flipbooks->FootstepFrames;
			uint8 CurrentWalkingFrame = GetFlipbookComponent()->GetPlaybackPositionInFrames();

			if (LastFootstepFrame != CurrentWalkingFrame && FootstepFrames->Contains(CurrentWalkingFrame))
			{
				LastFootstepFrame = CurrentWalkingFrame;
				MovementComponent->BroadcastFootstep();
			}
		}
	}
}

UBoxComponent* ABasePawn::GetCollisionComponent()
{
	return CollisionComponent;
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
	case EPlatformMovementMode::Flying:
		break;
	}

	if (IsAttacking())
	{
		Flipbook = AttackingFlipbook;
	}

	if (IsDying())
	{
		Flipbook = Flipbooks->Dying;
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
	FVector FinalKnockbackDirection = FVector(0.0f, KnockbackDirection.Y < 0.0f ? -1.0f : 1.0f, KnockbackDirection.Z < 0.0f ? -1.0f : 1.0f);
	const float TotalKnockback = GetKnockbackAmount() * KnockbackMultiplier;
	
	GetPlatformMovementComponent()->Knockback(FinalKnockbackDirection, TotalKnockback);
}

void ABasePawn::Damage(const float Damage)
{
	ICombatantInterface::Damage(Damage);
	UpdateFlipbook();

	OnTakeDamage(Health);
}

void ABasePawn::ResetPlatformActor()
{
	SetHealth(MaxHealth);

	Dying = false;
	Attacking = false;
	Moving = false;

	LastFootstepFrame = -1;

	UpdateFlipbook();

	for (int32 Index = 0; Index < AttackInfoArray.Num(); Index++)
	{
		FAttackInfo* AttackInfo = &AttackInfoArray[Index];
		AttackInfo->IsCooldown = false;
	}

	GetPlatformMovementComponent()->ResetComponent();
	
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

bool ABasePawn::Killed()
{
	if (!IsDying())
	{
		Dying = true;
		
		UPaperFlipbook* DyingFlipbook = nullptr;

		if (Flipbooks) DyingFlipbook = Flipbooks->Dying;
		
		float DyingDuration = 1.0f;
		if (DyingFlipbook) DyingDuration = DyingFlipbook->GetTotalDuration();

		FTimerHandle DyingTimerHandle;
		FTimerDelegate DyingTimerDelegate;

		DyingTimerDelegate.BindUFunction(this, FName("Killed"));
	
		GetWorldTimerManager().SetTimer(DyingTimerHandle, DyingTimerDelegate, DyingDuration, false, DyingDuration);
		return false;
	}

	return true;
}

float ABasePawn::GetKnockbackAmount()
{
	return KnockbackAmount;
}

void ABasePawn::SetHealth(const float InHealth)
{
	Health = InHealth;
}

void ABasePawn::CancelAttack(FTimerHandle& AttackTimerHandle, uint8 AttackID)
{
	if (GetWorldTimerManager().IsTimerActive(AttackTimerHandle)) GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	OnAttackFinished(AttackID);
}

void ABasePawn::OnAttackFinished(uint8 AttackID)
{
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

		OnFinishAttack(AttackID);
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

bool ABasePawn::IsAttackCoolingDown(uint8 AttackID)
{
	if (AttackID < AttackInfoArray.Num())
	{
		const FAttackInfo* AttackInfo = &AttackInfoArray[AttackID];
		return AttackInfo->IsCooldown;
	}
	return false;
}

bool ABasePawn::DoAttack(uint8 AttackID, FTimerHandle& TimerHandle, FTimerDelegate& Callback, uint8 BeginFrame, uint8 EndFrame, float& PlaybackBegin, float& PlaybackEnd)
{
	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();
	
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorldTimerManager().SetTimer(TimerHandle, Callback, 0.01f, true);

		FTimerHandle CancelAttackTimerHandle;
		FTimerDelegate CancelAttackTimerDelegate;
		float RemainingDuration = PlaybackMax - PlaybackCurrent;
		
		CancelAttackTimerDelegate.BindUFunction(this, FName("CancelAttack"), TimerHandle, AttackID);
		GetWorldTimerManager().SetTimer(CancelAttackTimerHandle, CancelAttackTimerDelegate, RemainingDuration, false, RemainingDuration);
		return false;
	}
	
	float Framerate = GetFlipbookComponent()->GetFlipbookFramerate();

	PlaybackBegin = BeginFrame / Framerate;
	PlaybackEnd = EndFrame / Framerate;

	if (PlaybackCurrent < PlaybackBegin || PlaybackCurrent > PlaybackEnd) return false;

	return true;
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
