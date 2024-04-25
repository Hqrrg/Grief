// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GriefCharacter.h"
#include "Interfaces/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class GRIEF_API AEnemyCharacter : public AGriefCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter(const FObjectInitializer& ObjectInitializer);

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

	UFUNCTION(BlueprintCallable)
	virtual bool Attack(uint8 AttackID) override;

private:
	virtual void Killed() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree = nullptr;
};
