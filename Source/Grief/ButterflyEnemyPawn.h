// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "ButterflyEnemyPawn.generated.h"

UENUM(BlueprintType)
enum class EButterflyPathType : uint8
{
	Figure8,
	Circle
};

UENUM(BlueprintType)
enum class EButterflyAttack : uint8
{
	Shoot
};

UCLASS()
class GRIEF_API AButterflyEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PathCollision = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	class USplineComponent* MovementPath = nullptr;

public:
	// Sets default values for this pawn's properties
	AButterflyEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool Attack(uint8 AttackID, bool StopMovement) override;

	UFUNCTION()
	void ButterflyShoot();

	virtual void OnAttackFinished(uint8 AttackID) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(EButterflyAttack InButterflyAttack) const {
		return static_cast<uint8>(InButterflyAttack);
	}

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat|Projectiles", meta = (AllowPrivateAccess = "true"))
	class AProjectileManager* FireballProjectileManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EButterflyPathType ButterflyPathType = EButterflyPathType::Figure8;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementPathRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sockets", DisplayName = "Fireball Origin", meta = (AllowPrivateAccess = "true"))
	FName FireballOriginSocketName = FName("FireballOrigin");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed = 3.0f;
	
	float Alpha = 0.0f;

	bool CanShoot = true;

	FTimerHandle FireballAttackTimerHandle;
	FTimerDelegate FireballAttackTimerDelegate;
};
