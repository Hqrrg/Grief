// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "InputActionValue.h"
#include "Interfaces/PlatformPlayer.h"
#include "PlayerPawn.generated.h"

UENUM(BlueprintType)
enum class EPlayerAttack : uint8
{
	Default = 0
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerKilled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerSpawned, bool, Respawned);

UCLASS()
class GRIEF_API APlayerPawn : public ABasePawn, public IPlatformPlayer
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPlatformCameraComponent* PlatformCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UAttackHitboxComponent* HighAttackHitbox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UAttackHitboxComponent* MiddleAttackHitbox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UAttackHitboxComponent* LowAttackHitbox = nullptr;
	
public:
	APlayerPawn();

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerKilled OnPlayerKilled;

	UPROPERTY(BlueprintAssignable)
	FPlayerSpawned OnPlayerSpawned;
	
protected:
	// Called when the actor is spawned
	virtual void BeginPlay() override;

	// Setup input bindings
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual bool Killed() override;

public:
	void Move(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);
	
	void Attack(const FInputActionValue& Value);

private:
	FORCEINLINE uint8 GetAttackID(EPlayerAttack InPlayerAttack) const { return static_cast<uint8>(InPlayerAttack); }
	
private:
	UFUNCTION()
	void DefaultAttack(class UAttackHitboxComponent* Hitbox);

	virtual void OnAttackFinished(uint8 AttackID) override;

public:
	virtual ICombatantInterface* GetCombatant() override;

	virtual ACheckpoint* GetCheckpoint() override;

	virtual void SetCheckpoint(ACheckpoint* InCheckpoint) override;

	virtual bool IsInvincible() override;

	virtual void Damage(const float Damage) override;

	virtual void ResetPlatformActor() override;

public:
	FORCEINLINE void BroadcastPlayerSpawned(bool Respawned) { OnPlayerSpawned.Broadcast(Respawned); }
	FORCEINLINE void BroadcastPlayerKilled() { OnPlayerKilled.Broadcast(); }
	
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPlatformCameraComponent* GetPlatformCameraComponent() const { return PlatformCameraComponent; }

private:
	UFUNCTION()
	void RemoveInvincibility();

private:
	UPROPERTY()
	ACheckpoint* Checkpoint = nullptr;

private:
	bool Invincible = false;
	float InvincibilityDuration = 1.0f;

	FTimerHandle DefaultAttackTimerHandle;
	FTimerDelegate DefaultAttackTimerDelegate;

	bool DefaultAttackTriggered = false;
};
