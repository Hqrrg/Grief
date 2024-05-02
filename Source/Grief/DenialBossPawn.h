// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossPawn.h"
#include "DenialBossPawn.generated.h"

UENUM(BlueprintType)
enum class EDenialBossAttack : uint8
{
	LaserBarrage,
	Hyperbeam,
	Slam
};

UCLASS()
class GRIEF_API ADenialBossPawn : public ABossPawn
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAttackHitboxComponent* SlamAttackHitbox = nullptr;

public:
	// Sets default values for this pawn's properties
	ADenialBossPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual bool Attack(uint8 AttackID) override;

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(EDenialBossAttack InDenialBossAttack) const { return static_cast<uint8>(InDenialBossAttack); }

private:
	UFUNCTION()
	void Attack_LaserBarrage();

	UFUNCTION()
	void Attack_Hyperbeam();

	UFUNCTION()
	void Attack_Slam();

	UFUNCTION()
	void FireLaser();

private:
	virtual bool Killed() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float LaserBarrageFireRate = 0.01f;
	
private:
	FTimerHandle LaserBarrageTimerHandle;
	FTimerHandle HyperbeamTimerHandle;
	FTimerHandle SlamTimerHandle;

	FTimerDelegate LaserBarrageTimerDelegate;
	FTimerDelegate HyperbeamTimerDelegate;
	FTimerDelegate SlamTimerDelegate;

	FTimerHandle FireLaserTimerHandle;
	FVector LaserBarrageTarget;
	
	bool HyperbeamFiring = false;
	FVector HyperbeamOrigin;
	FVector HyperbeamTarget;
	FVector HyperbeamStart;

	bool Slammed = false;
};
