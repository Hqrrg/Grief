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

	/*
	 * Work to be done here, probably best to create a base Character that both Player & Enemy can extend.
	 * That way the sprite logic, data table implementation and such doesnt need to be duplicated.
	 * Will do later.
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UHitboxComponent* AttackHitbox = nullptr;

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
	virtual float GetMaxHealth() override;
	
	UFUNCTION(BlueprintPure)
	virtual float GetHealth() override;

	UFUNCTION(BlueprintPure)
	virtual class UBehaviorTree* GetBehaviourTree() override;

	UFUNCTION(BlueprintPure)
	virtual bool IsObscured(const AActor* TargetActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void Knockback(const FVector OriginLocation, const float KnockbackMultiplier) override;

	UFUNCTION(BlueprintCallable)
	virtual void Attack(uint8 AttackID) override;

private:
	UFUNCTION(BlueprintPure)
	virtual float GetKnockbackAmount() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetMaxHealth(const float InMaxHealth) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(const float InHealth) override;

	virtual void Killed() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree = nullptr;

	UPROPERTY()
	float KnockbackAmount = 100.0f;
	
	UPROPERTY()
	float MaxHealth = 100.0f;
	
	UPROPERTY()
	float Health = MaxHealth;
};
