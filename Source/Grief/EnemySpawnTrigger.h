// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces\PlatformActorInterface.h"
#include "EnemySpawnTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemySpawnTriggerOverlapped);

UCLASS()
class GRIEF_API AEnemySpawnTrigger : public AActor, public IPlatformActorInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerBox = nullptr;

public:
	// Sets default values for this actor's properties
	AEnemySpawnTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FEnemySpawnTriggerOverlapped OnEnemySpawnTriggerOverlapped;
	
public:
	void BroadcastEnemySpawnTriggerOverlapped();

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	bool Triggered = false;

public:
	virtual void ResetPlatformActor() override;
};
