// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeEnemyPawn.h"
#include "AttackHitboxComponent.h"
#include "AttackAreaComponent.h"
#include "PaperFlipbookComponent.h"
#include "NiagaraComponent.h"
#include "PlayerPawn.h"

ASlimeEnemyPawn::ASlimeEnemyPawn() 
{
	LightAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("LightAttackHitbox"));
	LightAttackHitbox->SetupAttachment(GetFlipbookComponent());

	OutburstAttackArea = CreateDefaultSubobject<UAttackAreaComponent>(TEXT("OutburstAttackArea"));
	OutburstAttackArea->SetupAttachment(FlipbookComponent);

	OutburstNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OutburstNiagara"));
	OutburstNiagara->SetAutoActivate(false);
	OutburstNiagara->SetupAttachment(FlipbookComponent);
}

void ASlimeEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	OutburstNiagara->SetVariableFloat(FName("Size"), OutburstAttackArea->GetScaledSphereRadius() * 2);

	OutburstAttackTimerDelegate.BindUFunction(this, FName("ExplodeAttack"));
	KilledTimerDelegate.BindUFunction(this, FName("Killed"));
}

bool ASlimeEnemyPawn::Attack(uint8 AttackID, bool StopMovement)
{
	const bool ShouldAttack = Super::Attack(AttackID, StopMovement) && !KilledAttackDone;

	if (!ShouldAttack) return false;

	constexpr uint8 LightAttackID = static_cast<uint8>(ESlimeAttack::Light);
	constexpr uint8 ExplodeAttackID = static_cast<uint8>(ESlimeAttack::Explode);

	switch (AttackID)
	{
	case LightAttackID:
		ProxyAttack();
		break;

	case ExplodeAttackID:
		ExplodeAttack();
		break;

	default:
		break;
	}

	return true;
}

bool ASlimeEnemyPawn::Killed()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Attacking = false;
	
	if (!KilledAttackDone) 
	{
		Attack(GetAttackID(ESlimeAttack::Explode), true);

		float PlaybackLength = FlipbookComponent->GetFlipbookLength();

		GetWorldTimerManager().SetTimer(KilledTimerHandle, KilledTimerDelegate, PlaybackLength, false, PlaybackLength);

		GetFlipbookComponent()->SetHiddenInGame(true);

		KilledAttackDone = true;
		return true;

	}

	Destroy();
	return true;
}

void ASlimeEnemyPawn::ProxyAttack()
{
	const FAttackInfo LightAttackInfo = AttackInfoArray[GetAttackID(ESlimeAttack::Light)];

	TArray<AActor*> OverlappingPawns = LightAttackHitbox->GetContainedActors();


	if (OverlappingPawns.IsEmpty()) return;

	for (int32 Index = 0; Index < LightAttackHitbox->GetContainedActors().Num(); Index++)
	{
		if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(OverlappingPawns[Index]))
		{
			ICombatantInterface* Combatant = Player->GetCombatant();

			Combatant->Knockback(GetActorLocation(), LightAttackInfo.KnockbackMultiplier);
			Combatant->Damage(LightAttackInfo.Damage);
		}
	}
}

void ASlimeEnemyPawn::ExplodeAttack()
{
	float PlaybackBegin, PlaybackEnd;

	const uint8 AttackID = GetAttackID(ESlimeAttack::Explode);
	const FAttackInfo* OutburstAttackInfo = &AttackInfoArray[AttackID];

	if (!DoAttack(AttackID, OutburstAttackTimerHandle, OutburstAttackTimerDelegate, OutburstAttackInfo->BeginFrame, OutburstAttackInfo->EndFrame, PlaybackBegin, PlaybackEnd)) return;


	if (CanOutburst)
	{
		const float OutburstDuration = PlaybackEnd - PlaybackBegin;
		OutburstNiagara->SetVariableFloat(FName("Duration"), OutburstDuration);

		OutburstNiagara->Activate(true);

		TArray<AActor*> ContainedActors = OutburstAttackArea->GetContainedActors();

		for (int32 Index = 0; Index < ContainedActors.Num(); Index++)
		{
			if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(ContainedActors[Index]))
			{
				ICombatantInterface* Combatant = Player->GetCombatant();

				Combatant->Knockback(GetActorLocation(), OutburstAttackInfo->KnockbackMultiplier);
				Combatant->Damage(OutburstAttackInfo->Damage);
			}
		}
		CanOutburst = false;

		OnAttack(GetAttackID(ESlimeAttack::Explode));
	}
}

void ASlimeEnemyPawn::OnAttackFinished(uint8 AttackID)
{
	constexpr uint8 LightAttackID = static_cast<uint8>(ESlimeAttack::Light);
	constexpr uint8 ExplodeAttackID = static_cast<uint8>(ESlimeAttack::Explode);

	switch (AttackID)
	{
	case LightAttackID:
		break;

	case ExplodeAttackID:
		CanOutburst = true;
		break;

	default:
		break;
	}
}
