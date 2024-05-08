// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Interfaces/EnemyInterface.h"
#include "EnemyPawn.generated.h"

class AAIPatrolRoute;
class AMovementBoundingBox;

UCLASS()
class GRIEF_API AEnemyPawn : public ABasePawn, public IEnemyInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UPlayerSensingComponent* PlayerSensing = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent = nullptr;

public:
	// Sets default values for this character's properties
	AEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateMoving();
	
	UFUNCTION(BlueprintPure)
	FVector2D GetMovementVector();

	virtual void StopAttacking(uint8 AttackID) override;

	UFUNCTION(BlueprintPure)
	virtual class UBehaviorTree* GetBehaviourTree() override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE class AAIPatrolRoute* GetPatrolRoute() const { return PatrolRoute; }

	UFUNCTION(BlueprintCallable)
	virtual bool Attack(uint8 AttackID, bool StopMovement) override;

	FORCEINLINE void SetMovementBoundingBox(class AMovementBoundingBox* InMovementBoundingBox) { MovementBoundingBox = InMovementBoundingBox; } 

	virtual ICombatantInterface* GetCombatant() override;

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) override;

	virtual void ResetPlatformActor() override;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetSpawnParamaters(UEnemySpawnParamaters* InSpawnParamaters) { SpawnParamaters = InSpawnParamaters; }

protected:
	virtual bool Killed() override;

protected:
	UPROPERTY()
	UEnemySpawnParamaters* SpawnParamaters = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	AAIPatrolRoute* PatrolRoute = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	AMovementBoundingBox* MovementBoundingBox = nullptr;
};
