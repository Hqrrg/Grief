// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperFlipbook.h"
#include "PlatformPawnMovement.h"
#include "Enums/Direction.h"
#include "Interfaces/CombatantInterface.h"
#include "Interfaces\PlatformActorInterface.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<float> FootstepFrames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* Dying;
};

UCLASS()
class GRIEF_API ABasePawn : public APawn, public IPlatformActorInterface, public ICombatantInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* FlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UPlatformPawnMovement* MovementComponent = nullptr;
	
public:
	ABasePawn();

protected:
	// Called when the actor is spawned
	virtual void BeginPlay() override;

public:
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce = false) override;

public:
	virtual class UBoxComponent* GetCollisionComponent() override;

	FORCEINLINE class  UPlatformPawnMovement* GetPlatformMovementComponent() const { return MovementComponent; }
	
protected:
	FORCEINLINE class UPaperFlipbookComponent* GetFlipbookComponent() const { return FlipbookComponent; }

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginAttack(uint8 AttackID);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttack(uint8 AttackID);

	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishAttack(uint8 AttackID);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnKilled();
	
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetMovementDirection() const { return MovementDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetAttackDirection() const { return AttackDirection; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMovementDirection(EDirection Direction) { MovementDirection = Direction; } 
	
protected:
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAttacking() const { return Attacking; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsMoving() const { return Moving; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDying() const { return Dying; }

	UFUNCTION(BlueprintCallable)
	void UpdateDirections(const FVector2D MovementVector);

	UFUNCTION(BlueprintPure)
	virtual bool IsAttackCoolingDown(uint8 AttackID) override;
	
	UFUNCTION()
	virtual void StopAttacking(uint8 AttackID);

	UFUNCTION()
	void RemoveAttackCooldown(uint8 AttackID);

	bool DoAttack(uint8 AttackID, FTimerHandle& TimerHandle, FTimerDelegate& Callback, uint8 BeginFrame, uint8 EndFrame, float& PlaybackBegin, float& PlaybackEnd);

	UFUNCTION()
	void UpdateFlipbook();

public:
	UFUNCTION(BlueprintPure)
	virtual float GetMaxHealth() override;
	
	UFUNCTION(BlueprintPure)
	virtual float GetHealth() override;

	UFUNCTION(BlueprintPure)
	virtual bool IsObscured(const AActor* TargetActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void Knockback(const FVector OriginLocation, const float KnockbackMultiplier) override;

	virtual void Damage(const float Damage) override;

	virtual void ResetPlatformActor() override;

	UFUNCTION()
	virtual bool Killed() override;
	
protected:
	UFUNCTION(BlueprintPure)
	virtual float GetKnockbackAmount() override;

	virtual void SetHealth(const float InHealth) override;

private:
	UFUNCTION()
	void CancelAttack(FTimerHandle& AttackTimerHandle, uint8 AttackID);

	virtual void OnAttackFinished(uint8 AttackID);

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

	bool Dying = false;
	bool Attacking = false;
	bool Moving = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EDirection MovementDirection = EDirection::Right;
	
	EDirection AttackDirection;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float KnockbackAmount = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 6.0f;
	
	UPROPERTY()
	float Health = MaxHealth;

private:
	uint8 LastFootstepFrame = -1;
};
