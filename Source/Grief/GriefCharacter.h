// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"
#include "GriefCharacter.generated.h"

/**
 * 
 */

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDirection : uint8
{
	None	= 0b00000000,
	Up		= 0b00000001,
	Down	= 0b00000010,
	Left	= 0b00000100,
	Right	= 0b00001000,

	UpLeft		= Up | Left,
	UpRight		= Up | Right,
	DownLeft	= Down | Left,
	DownRight	= Down | Right
};
ENUM_CLASS_FLAGS(EDirection)

UENUM(BlueprintType)
enum class EPlatformerMovementMode : uint8
{
	Grounded = 0,
	Jumping,
	Falling
};

UCLASS()
class GRIEF_API AGriefCharacter : public APaperCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPlatformCameraComponent* PlatformCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;

public:
	AGriefCharacter(const FObjectInitializer& ObjectInitializer);

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
	FORCEINLINE void SetPlatformerMovementMode(const EPlatformerMovementMode InPlatformerMovementMode) { PlatformerMovementMode = InPlatformerMovementMode; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UPlatformCameraComponent* GetPlatformCameraComponent() const { return PlatformCameraComponent; }
	
private:
	void UpdateDirections(const FVector2D MovementVector);
	
private:
	UPROPERTY()
	EDirection MovementDirection = EDirection::Right;

	UPROPERTY()
	EDirection AttackDirection = EDirection::Right;

	UPROPERTY()
	EPlatformerMovementMode PlatformerMovementMode = EPlatformerMovementMode::Grounded;
};
