// Fill out your copyright notice in the Description page of Project Settings.


#include "DenialBossPawn.h"

#include "AttackHitboxComponent.h"
#include "CollisionDebugDrawingPublic.h"
#include "PaperFlipbookComponent.h"
#include "ProjectileManager.h"
#include "SimpleProjectile.h"
#include "Components/BoxComponent.h"
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

bool ADenialBossPawn::Attack(uint8 AttackID)
{
	const bool ShouldAttack = Super::Attack(AttackID);

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
	
	float PlaybackBegin, PlaybackEnd;

	FAttackInfo LaserBarrageAttackInfo = AttackInfoArray[GetAttackID(EDenialBossAttack::LaserBarrage)];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax)
	{
		if (GetWorldTimerManager().IsTimerActive(FireLaserTimerHandle)) GetWorldTimerManager().ClearTimer(FireLaserTimerHandle);
	}

	if (!DoAttack(LaserBarrageTimerHandle,LaserBarrageTimerDelegate, LaserBarrageAttackInfo.BeginFrame, LaserBarrageAttackInfo.EndFrame,PlaybackBegin, PlaybackEnd)) return;

	if (!GetWorldTimerManager().IsTimerActive(FireLaserTimerHandle))
	{
		GetWorldTimerManager().SetTimer(FireLaserTimerHandle, this, &ADenialBossPawn::FireLaser, LaserBarrageFireRate, true);
	}
}

void ADenialBossPawn::Attack_Hyperbeam()
{
	float PlaybackBegin, PlaybackEnd;

	FAttackInfo HyperbeamAttackInfo = AttackInfoArray[GetAttackID(EDenialBossAttack::Hyperbeam)];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax) HyperbeamFiring = false;

	if (!DoAttack(HyperbeamTimerHandle,HyperbeamTimerDelegate, HyperbeamAttackInfo.BeginFrame, HyperbeamAttackInfo.EndFrame,PlaybackBegin, PlaybackEnd)) return;
	
	if (!HyperbeamFiring)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
		HyperbeamOrigin = FlipbookComponent->GetSocketLocation(HyperbeamOriginSocketName);
		HyperbeamTarget = PlayerPawn->GetActorLocation();
		HyperbeamStart = FMath::Abs(((HyperbeamOrigin + HyperbeamTarget) / 2).Length()) >= 500.0f
			? (HyperbeamOrigin + HyperbeamTarget) / 2
			: GetActorLocation() + (HyperbeamTarget - HyperbeamOrigin).GetSafeNormal() * 500.0f;
		HyperbeamFiring = true;
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
			
			Combatant->ApplyDamage(HyperbeamAttackInfo.Damage);
			Combatant->Knockback(SweepResult->ImpactPoint, HyperbeamAttackInfo.KnockbackMultiplier);
		}
	}
}

void ADenialBossPawn::Attack_Slam()
{
	float PlaybackBegin, PlaybackEnd;

	FAttackInfo SlamAttackInfo = AttackInfoArray[GetAttackID(EDenialBossAttack::Slam)];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax) Slammed = false;
	
	if (!DoAttack(SlamTimerHandle,SlamTimerDelegate, SlamAttackInfo.BeginFrame, SlamAttackInfo.EndFrame,PlaybackBegin,PlaybackEnd)) return;

	if (!Slammed)
	{
		TArray<AActor*> ContainedActors = SlamAttackHitbox->GetContainedActors();

		for (int32 Index = 0; Index < ContainedActors.Num(); Index++)
		{
			if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(ContainedActors[Index]))
			{
				ICombatantInterface* Combatant = Player->GetCombatant();
				
				Combatant->ApplyDamage(SlamAttackInfo.Damage);
				Combatant->Knockback(GetActorLocation(), SlamAttackInfo.KnockbackMultiplier);
			}
		}
		Slammed = true;
	}
}

void ADenialBossPawn::FireLaser()
{
	FAttackInfo LaserBarrageAttackInfo = AttackInfoArray[GetAttackID(EDenialBossAttack::LaserBarrage)];
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	FVector LaserOrigin = FlipbookComponent->GetSocketLocation(LaserOriginSocketName);
	FVector LaserTarget = PlayerPawn->GetActorLocation();

	ASimpleProjectile* Laser = LaserProjectileManager->GetProjectile();
	Laser->SetAttackValues(LaserBarrageAttackInfo.Damage, LaserBarrageAttackInfo.KnockbackMultiplier);
	Laser->SetActorLocation(LaserOrigin);
	Laser->FireAt(LaserTarget);
}

bool ADenialBossPawn::Killed()
{
	bool Destroyed = Super::Killed();

	if (Destroyed)
	{
		// Spawn Anger
		return true;
	}
	return false;
}

