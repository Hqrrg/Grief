// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GriefGameModeBase.generated.h"

/**
 * 
 */

class APlayerPawn;
class APlayerController;
class ACheckpoint;

UCLASS()
class GRIEF_API AGriefGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

public:
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();
	
private:
	APlayerPawn* SpawnPlayer();
	ACheckpoint* FindDefaultCheckpoint();

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE APlayerPawn* GetPlayerRef() const { return PlayerRef; }
	
private:
	UPROPERTY()
	APlayerController* PlayerControllerRef = nullptr;
	
	UPROPERTY()
	APlayerPawn* PlayerRef = nullptr;
};
