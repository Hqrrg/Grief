// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatantInterface.h"


float ICombatantInterface::GetMaxHealth()
{
	return 0.0f;
}

float ICombatantInterface::GetHealth()
{
	return 0.0f;
}

float ICombatantInterface::GetKnockbackAmount()
{
	return 0.0f;
}

bool ICombatantInterface::IsObscured(const AActor* TargetActor)
{
	return false;
}

bool ICombatantInterface::ShouldKnockback()
{
	return GetKnockbackAmount() > 0.0f;
}

bool ICombatantInterface::Killed()
{
	return false;
}

void ICombatantInterface::ApplyDamage(float Damage)
{
	if (!IsAlive()) return;
	
	const float Health = GetHealth();
	const float MaxHealth = GetMaxHealth();
	
	const float NewHealth = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	SetHealth(NewHealth);

	if (!IsAlive()) Killed();
}

bool ICombatantInterface::IsAlive()
{
	return GetHealth() > 0.0f;
}