// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfEnemy.h"

#include "AttackHitboxComponent.h"
#include "PlatformCharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "PlayerCharacter.h"


// Sets default values
AWolfEnemy::AWolfEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	LightAttackHitbox = CreateDefaultSubobject<UAttackHitboxComponent>(TEXT("LightAttackHitbox"));
	LightAttackHitbox->SetupAttachment(FlipbookComponent);
}

// Called when the game starts or when spawned
void AWolfEnemy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWolfEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWolfEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AWolfEnemy::Attack(uint8 AttackID)
{
	const bool ShouldAttack = Super::Attack(AttackID);

	if (!ShouldAttack) return false;
	
	constexpr uint8 LightAttackID = static_cast<uint8>(EWolfAttack::Light);
	constexpr uint8 HeavyAttackID = static_cast<uint8>(EWolfAttack::Heavy);
	
	switch (AttackID)
	{
		case LightAttackID:
			LightAttack();
			break;
		
		case HeavyAttackID:
			//HeavyAttack();
			break;
		
		default:
			break;
	}
	
	return true;
}

void AWolfEnemy::LightAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString("Bye"));
	const FAttackInfo LightAttackInfo = AttackInfoArray[GetAttackID(EWolfAttack::Light)];
	
	TArray<AActor*> OverlappingCombatants = LightAttackHitbox->GetOverlappingCombatants();

	if (OverlappingCombatants.IsEmpty()) return;
	
	for (int32 Index = 0; Index < LightAttackHitbox->GetOverlappingCombatants().Num(); Index++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString("Hi"));
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OverlappingCombatants[Index]))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString("Hello"));
			PlayerCharacter->Knockback(GetActorLocation(), LightAttackInfo.KnockbackMultiplier);
		}
	}
}

