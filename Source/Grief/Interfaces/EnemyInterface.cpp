// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInterface.h"


float IEnemyInterface::GetMaxHealth()
{
	return 0.0f;
}

float IEnemyInterface::GetHealth()
{
	return 0.0f;
}

float IEnemyInterface::GetKnockbackAmount()
{
	return 0.0f;
}

UBehaviorTree* IEnemyInterface::GetBehaviourTree()
{
	return nullptr;
}

bool IEnemyInterface::IsObscured(const AActor* TargetActor)
{
	return false;
}

bool IEnemyInterface::ShouldKnockback()
{
	return GetKnockbackAmount() > 0.0f;
}

// Add default functionality here for any IEnemyInterface functions that are not pure virtual.
void IEnemyInterface::ApplyDamage(float Damage)
{
	if (!IsAlive()) return;
	
	const float Health = GetHealth();
	const float MaxHealth = GetMaxHealth();
	
	const float NewHealth = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	SetHealth(NewHealth);

	if (!IsAlive()) Killed();
}

bool IEnemyInterface::IsAlive()
{
	return GetHealth() > 0.0f;
}
