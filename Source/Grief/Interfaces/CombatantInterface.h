// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatantInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCombatantInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRIEF_API ICombatantInterface
{
	GENERATED_BODY()

public:
	virtual class UBoxComponent* GetCollisionComponent() = 0;
	
	virtual float GetMaxHealth();
	virtual float GetHealth();
	
	virtual bool IsObscured(const AActor* TargetActor);
	virtual void Knockback(const FVector OriginLocation, const float KnockbackMultiplier) = 0;
	virtual bool Killed();

	virtual bool IsAttackCoolingDown(uint8 AttackID);

	virtual bool IsInvincible();
	virtual void ApplyDamage(const float Damage);
	
	virtual bool IsAlive() final;
	virtual bool ShouldKnockback() final;

private:
	virtual float GetKnockbackAmount();
	virtual void SetHealth(const float InHealth) = 0;
};
