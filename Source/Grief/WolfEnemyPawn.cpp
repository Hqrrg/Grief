// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfEnemyPawn.h"

#include "AttackHitboxComponent.h"
#include "PaperFlipbookComponent.h"
#include "PlayerPawn.h"


// Sets default values
AWolfEnemyPawn::AWolfEnemyPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	LightAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("LightAttackHitbox"));
	LightAttackHitbox->SetupAttachment(GetFlipbookComponent());
}

// Called when the game starts or when spawned
void AWolfEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWolfEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AWolfEnemyPawn::Attack(uint8 AttackID, bool StopMovement)
{
	const bool ShouldAttack = Super::Attack(AttackID, StopMovement);

	if (!ShouldAttack) return false;
	
	constexpr uint8 LightAttackID = static_cast<uint8>(EWolfAttack::Light);
	
	switch (AttackID)
	{
		case LightAttackID:
			LightAttack();
			break;
		
		default:
			break;
	}
	
	return true;
}

void AWolfEnemyPawn::LightAttack()
{
	const FAttackInfo LightAttackInfo = AttackInfoArray[GetAttackID(EWolfAttack::Light)];
	
	TArray<AActor*> OverlappingPawns = LightAttackHitbox->GetContainedActors();

	if (OverlappingPawns.IsEmpty()) return;
	
	for (int32 Index = 0; Index < LightAttackHitbox->GetContainedActors().Num(); Index++)
	{
		if (IPlatformPlayer* Player = Cast<IPlatformPlayer>(OverlappingPawns[Index]))
		{
			ICombatantInterface* Combatant = Player->GetCombatant();

			Combatant->Knockback(GetActorLocation(), LightAttackInfo.KnockbackMultiplier);
			Combatant->ApplyDamage(LightAttackInfo.Damage);
		}
	}
}

