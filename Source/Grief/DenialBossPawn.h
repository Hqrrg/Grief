// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AngerBossPawn.h"
#include "BossPawn.h"
#include "DenialBossPawn.generated.h"

class UEnemySpawnParamaters;
class AProjectileManager;
class AEnemySpawner;

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
	virtual bool Attack(uint8 AttackID, bool StopMovement) override;

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(EDenialBossAttack InDenialBossAttack) const { return static_cast<uint8>(InDenialBossAttack); }


private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat|Projectiles", meta = (AllowPrivateAccess = "true"))
	AProjectileManager* LaserProjectileManager = nullptr;
	
private:
	UFUNCTION()
	void Attack_LaserBarrage();

	UFUNCTION()
	void Attack_Hyperbeam();

	UFUNCTION()
	void Attack_Slam();

	UFUNCTION()
	void FireLaser();

	virtual void OnAttackFinished(uint8 AttackID) override;

private:
	virtual bool Killed() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float LaserBarrageFireRate = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sockets", DisplayName = "Laser Origin", meta = (AllowPrivateAccess = "true"))
	FName LaserOriginSocketName = FName("LaserOrigin");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sockets", DisplayName = "Hyperbeam Origin", meta = (AllowPrivateAccess = "true"))
	FName HyperbeamOriginSocketName = FName("HyperbeamOrigin");

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Anger, meta = (AllowPrivateAccess = "true"))
	AEnemySpawner* AngerSpawner = nullptr;
	
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
