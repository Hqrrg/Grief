// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSensingComponent.generated.h"


UENUM(BlueprintType)
enum class ESensingRange : uint8
{
	None,
	Detection,
	Search
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerSensed, class APlayerPawn*, Player, bool, Detected, ESensingRange, SensingRange, bool, HasLineOfSight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerEscaped, class APlayerPawn*, Player);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIEF_API UPlayerSensingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerSensingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void StartTimer(float Interval);

private:
	UFUNCTION()
	void Update();

public:
	FORCEINLINE float GetDetectionRadius() const { return DetectionRadius; }

	FORCEINLINE float GetSearchRadius() const { return SearchRadius; }

	FORCEINLINE bool IsEnabled() const { return Enabled; }

	void BroadcastPlayerSensed(ESensingRange SensingRange);

	void BroadcastPlayerEscaped();

private:
	bool HasLineOfSight();

private:
	UPROPERTY(BlueprintAssignable)
	FPlayerSensed OnPlayerSensed;

	UPROPERTY(BlueprintAssignable)
	FPlayerEscaped OnPlayerEscaped;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Detection, meta = (AllowPrivateAccess = "true"))
	bool Enabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Detection, meta = (ClampMin = "32", AllowPrivateAccess))
	float DetectionRadius = 1024.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Detection,  meta = (ClampMin = "32", AllowPrivateAccess))
	float SearchRadius = 2048.0f;

private:
	FTimerHandle TimerHandle;
	float TimerInterval = 0.5f;

	UPROPERTY()
	class APlayerController* PlayerController = nullptr;

	UPROPERTY()
	class APlayerPawn* PlayerPawn = nullptr;

	bool bPlayerSensed;
	bool bPlayerDetected;

	ESensingRange LastSensingRange = ESensingRange::None;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
