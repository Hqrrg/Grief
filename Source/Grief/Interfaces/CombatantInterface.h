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
	virtual float GetMaxHealth();
	virtual float GetHealth();
	
	virtual bool IsObscured(const AActor* TargetActor);
	virtual void Knockback(const FVector OriginLocation, const float KnockbackMultiplier) = 0;
	virtual void Killed();

	virtual void ApplyDamage(const float Damage) final;
	virtual bool IsAlive() final;
	virtual bool ShouldKnockback() final;

private:
	virtual float GetKnockbackAmount();
	virtual void SetMaxHealth(const float InMaxHealth) = 0;
	virtual void SetHealth(const float InHealth) = 0;
};
