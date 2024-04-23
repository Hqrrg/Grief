// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Engine/DataTable.h"
#include "PaperFlipbook.h"
#include "Enums/Direction.h"
#include "GriefCharacter.generated.h"

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
class GRIEF_API AGriefCharacter : public APaperCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* FlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UPlatformCharacterMovementComponent* PlatformCharacterMovementComponent = nullptr;
	
public:
	AGriefCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the actor is spawned
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetMovementDirection() const { return MovementDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EDirection GetAttackDirection() const { return AttackDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EPlatformerMovementMode GetPlatformerMovementMode() const { return PlatformerMovementMode; }

	UFUNCTION(BlueprintCallable)
	void SetPlatformerMovementMode(const EPlatformerMovementMode InPlatformerMovementMode);
	
protected:
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAttacking() const { return Attacking; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsMoving() const { return Moving; }

	UFUNCTION()
	void StopAttacking();
	
	void UpdateDirections(const FVector2D MovementVector);

	void UpdateFlipbook();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	FName FlipbookKey;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	UDataTable* FlipbookDataTable = nullptr;
	
	FCharacterFlipbooks* Flipbooks;
	
	bool Attacking = false;
	bool Moving = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MeleeDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MeleeKnockbackMultiplier = 1.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EDirection MovementDirection = EDirection::Right;
	
	EDirection AttackDirection;
	
	EPlatformerMovementMode PlatformerMovementMode = EPlatformerMovementMode::Grounded;
};
