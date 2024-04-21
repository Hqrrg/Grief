// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "PaperFlipbook.h"
#include "PlayerCharacter.generated.h"

enum class EDirection : uint8;
/**
 * 
 */
UENUM(BlueprintType)
enum class EPlatformerMovementMode : uint8
{
	Grounded = 0,
	Jumping,
	Falling
};

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
	UPaperFlipbook* Attacking;
};

UCLASS()
class GRIEF_API APlayerCharacter : public APaperCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* FlipbookComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPlatformCameraComponent* PlatformCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UHitboxComponent* HighHitbox = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UHitboxComponent* MiddleHitbox = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UHitboxComponent* LowHitbox = nullptr;
	
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the actor is spawned
	virtual void BeginPlay() override;

	// Setup input bindings
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	void Move(const FInputActionValue& Value);
	
	void Attack(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetMovementDirection() const { return MovementDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetAttackDirection() const { return AttackDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EPlatformerMovementMode GetPlatformerMovementMode() const { return PlatformerMovementMode; }

	UFUNCTION(BlueprintCallable)
	void SetPlatformerMovementMode(const EPlatformerMovementMode InPlatformerMovementMode);

	UFUNCTION(BlueprintPure)
	FORCEINLINE UPlatformCameraComponent* GetPlatformCameraComponent() const { return PlatformCameraComponent; }
	
private:
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAttacking() const { return Attacking; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsMoving() const { return Moving; }

	UFUNCTION()
	void StopAttacking();
	
	void UpdateDirections(const FVector2D MovementVector);

	void UpdateFlipbook();
	
private:
	bool Attacking = false;
	bool Moving = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MeleeDamage = 10.0f;
	
	UPROPERTY()
	EDirection MovementDirection;

	UPROPERTY()
	EDirection AttackDirection;

	UPROPERTY()
	EPlatformerMovementMode PlatformerMovementMode = EPlatformerMovementMode::Grounded;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	UDataTable* FlipbookDataTable = nullptr;
	
	FCharacterFlipbooks* Flipbooks;
};
