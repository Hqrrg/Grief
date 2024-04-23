// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GriefCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GRIEF_API APlayerCharacter : public AGriefCharacter
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
	class UHitboxComponent* HighHitbox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
	class UHitboxComponent* MiddleHitbox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hitbox, meta = (AllowPrivateAccess = "true"))
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
	FORCEINLINE UPlatformCameraComponent* GetPlatformCameraComponent() const { return PlatformCameraComponent; }
};
