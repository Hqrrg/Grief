// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class GRIEF_API IEnemyInterface
{
	GENERATED_BODY()

public:
	virtual class UBehaviorTree* GetBehaviourTree();
	virtual bool Attack(uint8 AttackID) = 0;

	virtual ICombatantInterface* GetCombatant() = 0;
};
