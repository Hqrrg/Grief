// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatantInterface.h"
#include "UObject/Interface.h"
#include "PlatformPlayer.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlatformPlayer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRIEF_API IPlatformPlayer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ICombatantInterface* GetCombatant() = 0;
};