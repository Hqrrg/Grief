// Fill out your copyright notice in the Description page of Project Settings.


#include "AngerBossPawn.h"

#include "AttackAreaComponent.h"
#include "CollisionDebugDrawingPublic.h"
#include "PaperFlipbookComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Interfaces/PlatformPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "ProjectileManager.h"


// Sets default values
AAngerBossPawn::AAngerBossPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	OutburstAttackArea = CreateDefaultSubobject<UAttackAreaComponent>(TEXT("OutburstAttackArea"));
	OutburstAttackArea->SetupAttachment(FlipbookComponent);
	
	BeamRail = CreateDefaultSubobject<USplineComponent>(TEXT("BeamRail"));
	BeamRail->SetClosedLoop(true);
	BeamRail->SetupAttachment(CollisionComponent);

	OutburstNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OutburstNiagara"));
	OutburstNiagara->SetupAttachment(FlipbookComponent);

	GetPlatformMovementComponent()->SetFlying();
}

// Called when the game starts or when spawned
void AAngerBossPawn::BeginPlay()
{
	Super::BeginPlay();

	FireballAttackTimerDelegate.BindUFunction(this, FName("Attack_Fireball"));
	BeamAttackTimerDelegate.BindUFunction(this, FName("Attack_Beam"));
	OutburstAttackTimerDelegate.BindUFunction(this, FName("Attack_Outburst"));
}

void AAngerBossPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BeamRail->ClearSplinePoints();

	const FVector PointLocationZ = FVector(0.0f, 0.0f, BeamRailRadius);
	const FVector PointLocationY = FVector(0.0f, BeamRailRadius, 0.0f);

	BeamRail->AddSplineLocalPoint(PointLocationY);
	BeamRail->AddSplineLocalPoint(PointLocationZ);
	BeamRail->AddSplineLocalPoint(-PointLocationY);
	BeamRail->AddSplineLocalPoint(-PointLocationZ);

	float TangentFactor = 4 * (sqrt(2) - 1) / 3 + 1;

	FVector YTangent = FVector(0.0f, BeamRailRadius * TangentFactor, 0.0f);
	FVector ZTangent = FVector(0.0f, 0.0f, BeamRailRadius * TangentFactor);

	BeamRail->SetTangentAtSplinePoint(0, ZTangent, ESplineCoordinateSpace::Local);
	BeamRail->SetTangentAtSplinePoint(1, -YTangent, ESplineCoordinateSpace::Local);
	BeamRail->SetTangentAtSplinePoint(2, -ZTangent, ESplineCoordinateSpace::Local);
	BeamRail->SetTangentAtSplinePoint(3, YTangent, ESplineCoordinateSpace::Local);
}

bool AAngerBossPawn::Attack(uint8 AttackID, bool StopMovement)
{
	const bool ShouldAttack = Super::Attack(AttackID, StopMovement);

	if (!ShouldAttack) return false;

	constexpr uint8 FireballAttackID = static_cast<uint8>(EAngerBossAttack::Fireball);
	constexpr uint8 BeamAttackID = static_cast<uint8>(EAngerBossAttack::Beam);
	constexpr uint8 OutburstAttackID = static_cast<uint8>(EAngerBossAttack::Outburst);
	
	switch (AttackID)
	{
	case FireballAttackID:
		Attack_Fireball();
		break;
		
	case BeamAttackID:
		Attack_Beam();
		break;
		
	case OutburstAttackID:
		Attack_Outburst();
		break;
		
	default:
		break;
	}

	return true;
}

void AAngerBossPawn::Attack_Fireball()
{
	if (!FireballProjectileManager) return;
	
	float PlaybackBegin, PlaybackEnd;

	FAttackInfo FireballAttackInfo = AttackInfoArray[GetAttackID(EAngerBossAttack::Fireball)];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax) CanFireball = true;

	if (!DoAttack(FireballAttackTimerHandle,FireballAttackTimerDelegate,
		FireballAttackInfo.BeginFrame, FireballAttackInfo.EndFrame,
		PlaybackBegin, PlaybackEnd)) return;

	if (CanFireball)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		FVector OriginLocation = GetFlipbookComponent()->GetSocketLocation(FireballOriginSocketName);
		FVector TargetLocation = PlayerPawn->GetActorLocation();

		ASimpleProjectile* Fireball = FireballProjectileManager->GetProjectile();
		Fireball->SetAttackValues(FireballAttackInfo.Damage, FireballAttackInfo.KnockbackMultiplier);
		Fireball->SetActorLocation(OriginLocation);
		Fireball->FireAt(TargetLocation);
		
		CanFireball = false;
	}
	
}

void AAngerBossPawn::Attack_Beam()
{
	float PlaybackBegin, PlaybackEnd;

	FAttackInfo BeamAttackInfo = AttackInfoArray[GetAttackID(EAngerBossAttack::Beam)];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax)
	{
		BeamRail->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	
	if (!DoAttack(BeamAttackTimerHandle,BeamAttackTimerDelegate,
		BeamAttackInfo.BeginFrame, BeamAttackInfo.EndFrame,
		PlaybackBegin, PlaybackEnd)) return;

	BeamRail->AddRelativeRotation(FRotator(0.0f, 0.0f, BeamRotationSpeed), true);
	
	float SplineLength = BeamRail->GetSplineLength();

	for (int32 Index = 1; Index <= BeamCount; Index++)
	{
		FVector Origin = BeamRail->GetLocationAtDistanceAlongSpline(SplineLength * Index / BeamCount, ESplineCoordinateSpace::World);
		FVector ForwardVector = (Origin - GetActorLocation()).GetSafeNormal();
		float TraceLength = 5000.0f;

		FVector TraceEnd = Origin + ForwardVector * TraceLength;

		FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(10.0f, 10.0f, 10.0f));
	
		FHitResult* SweepResult = new FHitResult();
		FCollisionQueryParams QueryParams; QueryParams.AddIgnoredActor(this);
	
		bool IsBlocking = GetWorld()->SweepSingleByChannel(
			*SweepResult,
			Origin,
			TraceEnd,
			GetActorRotation().Quaternion(),
			ECC_Visibility,
			CollisionShape,
			QueryParams);

		TArray<FHitResult> Results;
		Results.Add(*SweepResult);
		
		DrawBoxSweeps(GetWorld(), Origin, TraceEnd, CollisionShape.GetExtent(), GetActorRotation().Quaternion(), Results, 0.01f);
	}
}

void AAngerBossPawn::Attack_Outburst()
{
	float PlaybackBegin, PlaybackEnd;

	FAttackInfo OutburstAttackInfo = AttackInfoArray[GetAttackID(EAngerBossAttack::Outburst)];

	float PlaybackCurrent = GetFlipbookComponent()->GetPlaybackPosition();
	float PlaybackMax = GetFlipbookComponent()->GetFlipbookLength();

	if (PlaybackCurrent >= PlaybackMax) CanOutburst = true;

	if (!DoAttack(OutburstAttackTimerHandle,OutburstAttackTimerDelegate,
		OutburstAttackInfo.BeginFrame, OutburstAttackInfo.EndFrame,
		PlaybackBegin, PlaybackEnd)) return;

	if (CanOutburst)
	{
		TArray<AActor*> ContainedActors = OutburstAttackArea->GetContainedActors();

		for (int32 Index = 0; Index < ContainedActors.Num(); Index++)
		{
			if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(ContainedActors[Index]))
			{
				ICombatantInterface* Combatant = Player->GetCombatant();

				Combatant->Knockback(GetActorLocation(), OutburstAttackInfo.KnockbackMultiplier);
				Combatant->ApplyDamage(OutburstAttackInfo.Damage);
			}
		}
		CanOutburst = false;
	}
}


