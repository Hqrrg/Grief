// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperFlipbook.h"
#include "PlatformPawnMovement.h"
#include "Enums/Direction.h"
#include "Interfaces/CombatantInterface.h"
#include "Structs\AttackInfo.h"
#include "BasePawn.generated.h"

enum class EDirection : uint8;

USTRUCT(BlueprintType)
struct FCharacterFlipbooks : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* Jumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* Falling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* Idling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* Walking;
};

UCLASS()
class GRIEF_API ABasePawn : public APawn, public ICombatantInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* FlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UPlatformPawnMovement* MovementComponent = nullptr;
	
public:
	ABasePawn();

protected:
	// Called when the actor is spawned
	virtual void BeginPlay() override;

public:
	virtual class UBoxComponent* GetCollisionComponent() override;

	FORCEINLINE class  UPlatformPawnMovement* GetPlatformMovementComponent() const { return MovementComponent; }
	
protected:
	FORCEINLINE class UPaperFlipbookComponent* GetFlipbookComponent() const { return FlipbookComponent; }
	
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetMovementDirection() const { return MovementDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetAttackDirection() const { return AttackDirection; }
	
protected:
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAttacking() const { return Attacking; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsMoving() const { return Moving; }

	UFUNCTION(BlueprintCallable)
	void UpdateDirections(const FVector2D MovementVector);
	
	UFUNCTION()
	virtual void StopAttacking(uint8 AttackID);

	UFUNCTION()
	void RemoveAttackCooldown(uint8 AttackID);

	UFUNCTION()
	void UpdateFlipbook();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	FName FlipbookKey;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	UDataTable* FlipbookDataTable = nullptr;
	
	FCharacterFlipbooks* Flipbooks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UDataTable* AttacksDataTable = nullptr;
	
	TArray<FAttackInfo> AttackInfoArray;

	UPROPERTY()
	class UPaperFlipbook* AttackingFlipbook = nullptr;
	
	bool Attacking = false;
	bool Moving = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EDirection MovementDirection = EDirection::Right;
	
	EDirection AttackDirection;

public:
	UFUNCTION(BlueprintPure)
	virtual float GetMaxHealth() override;
	
	UFUNCTION(BlueprintPure)
	virtual float GetHealth() override;

	UFUNCTION(BlueprintPure)
	virtual bool IsObscured(const AActor* TargetActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void Knockback(const FVector OriginLocation, const float KnockbackMultiplier) override;
	
private:
	UFUNCTION(BlueprintPure)
	virtual float GetKnockbackAmount() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetMaxHealth(const float InMaxHealth) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(const float InHealth) override;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float KnockbackAmount = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;
	
	UPROPERTY()
	float Health = MaxHealth;
};
