// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Interfaces/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class GRIEF_API AEnemyCharacter : public APaperCharacter, public IEnemyInterface
{
	GENERATED_BODY()

	/*
	 * Work to be done here, probably best to create a base Character that both Player & Enemy can extend.
	 * That way the sprite logic, data table implementation and such doesnt need to be duplicated.
	 * Will do later.
	 */

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	virtual float GetMaxHealth() override;
	
	UFUNCTION(BlueprintPure)
	virtual float GetHealth() override;

	UFUNCTION(BlueprintPure)
	virtual bool IsObscured(const AActor* TargetActor) override;

private:
	UFUNCTION(BlueprintCallable)
	virtual void SetMaxHealth(const float InMaxHealth) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(const float InHealth) override;

	virtual void Killed() override;
	
private:
	UPROPERTY()
	float MaxHealth = 100.0f;
	
	UPROPERTY()
	float Health = MaxHealth;
};
