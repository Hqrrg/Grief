// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "WolfEnemyPawn.generated.h"

UENUM(BlueprintType)
enum class EWolfAttack : uint8
{
	Light = 0,
	Heavy = 1
};

UCLASS()
class GRIEF_API AWolfEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UAttackHitboxComponent* LightAttackHitbox = nullptr;

public:
	// Sets default values for this character's properties
	AWolfEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual bool Attack(uint8 AttackID) override;

private:
	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(EWolfAttack InWolfAttack) const { return static_cast<uint8>(InWolfAttack); }

	void LightAttack();
};
