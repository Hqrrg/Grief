// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Interfaces/EnemyInterface.h"
#include "EnemyPawn.generated.h"

UCLASS()
class GRIEF_API AEnemyPawn : public ABasePawn, public IEnemyInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UPlayerSensingComponent* PlayerSensing = nullptr;

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

	UFUNCTION(BlueprintPure)
	virtual class UBehaviorTree* GetBehaviourTree() override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE class AAIPatrolRoute* GetPatrolRoute() const { return PatrolRoute; }

	UFUNCTION(BlueprintCallable)
	virtual bool Attack(uint8 AttackID) override;

private:
	virtual void Killed() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class AAIPatrolRoute* PatrolRoute = nullptr;
};
