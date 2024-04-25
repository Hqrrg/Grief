// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "WolfEnemy.generated.h"

UENUM(BlueprintType)
enum class EWolfAttack : uint8
{
	Light = 0,
	Heavy = 1
};

UCLASS()
class GRIEF_API AWolfEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UAttackHitboxComponent* LightAttackHitbox = nullptr;

public:
	// Sets default values for this character's properties
	AWolfEnemy(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual bool Attack(uint8 AttackID) override;

private:
	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(EWolfAttack InWolfAttack) const { return static_cast<uint8>(InWolfAttack); }

	void LightAttack();
};
