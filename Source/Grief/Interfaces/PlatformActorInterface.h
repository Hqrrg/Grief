// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlatformActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlatformActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRIEF_API IPlatformActorInterface
{
	GENERATED_BODY()

public:
	virtual void ResetPlatformActor() = 0;
};
