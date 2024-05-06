// Fill out your copyright notice in the Description page of Project Settings.


#include "DenialBossPawn.h"

#include "AngerBossPawn.h"
#include "AttackHitboxComponent.h"
#include "CollisionDebugDrawingPublic.h"
#include "PaperFlipbookComponent.h"
#include "ProjectileManager.h"
#include "SimpleProjectile.h"
#include "Interfaces/PlatformPlayer.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADenialBossPawn::ADenialBossPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SlamAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("SlamAttackArea"));
	SlamAttackHitbox->SetupAttachment(FlipbookComponent);
}

// Called when the game starts or when spawned
void ADenialBossPawn::BeginPlay()
{
	Super::BeginPlay();

	LaserBarrageTimerDelegate.BindUFunction(this, FName("Attack_LaserBarrage"));
	HyperbeamTimerDelegate.BindUFunction(this, FName("Attack_Hyperbeam"));
	SlamTimerDelegate.BindUFunction(this, FName("Attack_Slam"));
}

bool ADenialBossPawn::Attack(uint8 AttackID, bool StopMovement)
{
	const bool ShouldAttack = Super::Attack(AttackID, StopMovement);

	if (!ShouldAttack) return false;

	constexpr uint8 LaserBarrageAttackID = static_cast<uint8>(EDenialBossAttack::LaserBarrage);
	constexpr uint8 HyperbeamAttackID = static_cast<uint8>(EDenialBossAttack::Hyperbeam);
	constexpr uint8 SlamAttackID = static_cast<uint8>(EDenialBossAttack::Slam);

	switch (AttackID)
	{
	case LaserBarrageAttackID:
		Attack_LaserBarrage();
		break;
		
	case HyperbeamAttackID:
		Attack_Hyperbeam();
		break;
		
	case SlamAttackID:
		Attack_Slam();
		break;
		
	default:
		break;
	}

	return true;
}

void ADenialBossPawn::Attack_LaserBarrage()
{
	if (!LaserProjectileManager) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector ActorLocation = GetActorLocation();
	FVector Direction = (PlayerLocation - ActorLocation).GetSafeNormal();
	FVector2D MovementVector = FVector2D(Direction.Y, 0.0f);
	UpdateDirections(MovementVector);
	UpdateFlipbook();
	
	float PlaybackBegin, PlaybackEnd;

	const uint8 AttackID = GetAttackID(EDenialBossAttack::LaserBarrage);
	const FAttackInfo* LaserBarrageAttackInfo = &AttackInfoArray[AttackID];

	if (!DoAttack(AttackID, LaserBarrageTimerHandle,LaserBarrageTimerDelegate, LaserBarrageAttackInfo->BeginFrame, LaserBarrageAttackInfo->EndFrame,PlaybackBegin, PlaybackEnd)) return;

	if (!GetWorldTimerManager().IsTimerActive(FireLaserTimerHandle))
	{
		GetWorldTimerManager().SetTimer(FireLaserTimerHandle, this, &ADenialBossPawn::FireLaser, LaserBarrageFireRate, true);

		OnAttack(AttackID);
	}
}

void ADenialBossPawn::Attack_Hyperbeam()
{
	float PlaybackBegin, PlaybackEnd;

	const uint8 AttackID = GetAttackID(EDenialBossAttack::Hyperbeam);
	const FAttackInfo* HyperbeamAttackInfo = &AttackInfoArray[AttackID];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	
	if (!HyperbeamFiring)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		HyperbeamOrigin = FlipbookComponent->GetSocketLocation(HyperbeamOriginSocketName);
		HyperbeamTarget = PlayerPawn->GetActorLocation();
		HyperbeamStart = FMath::Abs(((HyperbeamOrigin + HyperbeamTarget) / 2).Length()) >= 500.0f
			? (HyperbeamOrigin + HyperbeamTarget) / 2
			: GetActorLocation() + (HyperbeamTarget - HyperbeamOrigin).GetSafeNormal() * 500.0f;
	}

	if (!DoAttack(AttackID, HyperbeamTimerHandle,HyperbeamTimerDelegate, HyperbeamAttackInfo->BeginFrame, HyperbeamAttackInfo->EndFrame,PlaybackBegin, PlaybackEnd)) return;

	if (!HyperbeamFiring)
	{
		HyperbeamFiring = true;
		
		OnAttack(AttackID);
	}
	
	float Alpha = FMath::GetMappedRangeValueClamped(FVector2D(PlaybackBegin, PlaybackEnd), FVector2D(0.0f, 1.0f), PlaybackCurrent);
	float TargetLocY = FMath::Lerp(HyperbeamStart.Y, HyperbeamTarget.Y, Alpha);
	float TargetLocZ = FMath::Lerp(0.0f, 300.0f, Alpha);
	FVector TargetLoc = FVector(0.0f, TargetLocY, TargetLocZ);
	FVector ForwardVector = (TargetLoc - HyperbeamOrigin).GetSafeNormal();

	FVector TraceEnd = HyperbeamOrigin + ForwardVector * 5000.0f;

	FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(10.0f, 10.0f, 10.0f));
	
	FHitResult* SweepResult = new FHitResult();
	FCollisionQueryParams QueryParams; QueryParams.AddIgnoredActor(this);
	
	bool IsBlocking = GetWorld()->SweepSingleByChannel(
		*SweepResult,
		HyperbeamOrigin,
		TraceEnd,
		GetActorRotation().Quaternion(),
		ECC_Visibility,
		CollisionShape,
		QueryParams);

	TArray<FHitResult> Results;
	Results.Add(*SweepResult);
		
	DrawBoxSweeps(GetWorld(), HyperbeamOrigin, TraceEnd, CollisionShape.GetExtent(), GetActorRotation().Quaternion(), Results, 0.01f);
	
	if (IsBlocking)
	{
		if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(SweepResult->GetActor()))
		{
			ICombatantInterface* Combatant = Player->GetCombatant();

			Combatant->Knockback(SweepResult->ImpactPoint, HyperbeamAttackInfo->KnockbackMultiplier);
			Combatant->ApplyDamage(HyperbeamAttackInfo->Damage);
		}
	}
}

void ADenialBossPawn::Attack_Slam()
{
	float PlaybackBegin, PlaybackEnd;

	const uint8 AttackID = GetAttackID(EDenialBossAttack::Slam);
	const FAttackInfo* SlamAttackInfo = &AttackInfoArray[AttackID];
	
	if (!DoAttack(AttackID, SlamTimerHandle,SlamTimerDelegate, SlamAttackInfo->BeginFrame, SlamAttackInfo->EndFrame,PlaybackBegin,PlaybackEnd)) return;

	if (!Slammed)
	{
		TArray<AActor*> ContainedActors = SlamAttackHitbox->GetContainedActors();

		for (int32 Index = 0; Index < ContainedActors.Num(); Index++)
		{
			if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(ContainedActors[Index]))
			{
				ICombatantInterface* Combatant = Player->GetCombatant();

				Combatant->Knockback(GetActorLocation(), SlamAttackInfo->KnockbackMultiplier);
				Combatant->ApplyDamage(SlamAttackInfo->Damage);
			}
		}
		Slammed = true;

		OnAttack(AttackID);
	}
}

void ADenialBossPawn::FireLaser()
{
	const FAttackInfo* LaserBarrageAttackInfo = &AttackInfoArray[GetAttackID(EDenialBossAttack::LaserBarrage)];
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	FVector LaserOrigin = FlipbookComponent->GetSocketLocation(LaserOriginSocketName);
	FVector LaserTarget = PlayerPawn->GetActorLocation();

	ASimpleProjectile* Laser = LaserProjectileManager->GetProjectile();
	Laser->SetAttackValues(LaserBarrageAttackInfo->Damage, LaserBarrageAttackInfo->KnockbackMultiplier);
	Laser->SetActorLocation(LaserOrigin);
	Laser->FireAt(LaserTarget);
}

void ADenialBossPawn::OnAttackFinished(uint8 AttackID)
{
	constexpr uint8 LaserBarrageAttackID = static_cast<uint8>(EDenialBossAttack::LaserBarrage);
	constexpr uint8 HyperbeamAttackID = static_cast<uint8>(EDenialBossAttack::Hyperbeam);
	constexpr uint8 SlamAttackID = static_cast<uint8>(EDenialBossAttack::Slam);

	switch (AttackID)
	{
	case LaserBarrageAttackID:
		if (GetWorldTimerManager().IsTimerActive(FireLaserTimerHandle)) GetWorldTimerManager().ClearTimer(FireLaserTimerHandle);
		break;
		
	case HyperbeamAttackID:
		HyperbeamFiring = false;
		break;
		
	case SlamAttackID:
		Slammed = false;
		break;
		
	default:
		break;
	}
}

bool ADenialBossPawn::Killed()
{
	bool Destroyed = Super::Killed();

	if (Destroyed)
	{
		UWorld* World = GetWorld();

		if (!World) return false;
		if (!AngerBossClass) return false;
		
		FTransform SpawnTransform = GetActorTransform();
		AAngerBossPawn* AngerBoss = World->SpawnActorDeferred<AAngerBossPawn>(AngerBossClass->GetAuthoritativeClass(), SpawnTransform);

		if (AngerBoss)
		{
			AngerBoss->FinishSpawning(SpawnTransform);
			
			if (AngerMovementBoundingBox)
			{
				AngerBoss->SetMovementBoundingBox(AngerMovementBoundingBox);
			}
		}
		
		return true;
	}
	return false;
}

