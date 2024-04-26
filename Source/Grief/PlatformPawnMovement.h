// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PlatformPawnMovement.generated.h"

UENUM(BlueprintType)
enum class EPlatformMovementMode : uint8
{
	Walking = 0,
	Jumping,
	Falling
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementModeUpdated);

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class GRIEF_API UPlatformPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()


public:
	UPlatformPawnMovement();
	
protected:
	// Initialize the component
	virtual void InitializeComponent() override;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Jump();
	void StopJumping();
	void SetFalling();
	void Knockback(FVector InKnockbackVector, float InKnockbackVelocity);

public:
	// Getter (MovementMode)
	FORCEINLINE EPlatformMovementMode GetMovementMode() const { return MovementMode; }

	// Setter (MovementMode)
	void SetMovementMode(const EPlatformMovementMode InMovementMode);

	// Getter (Jumping)
	FORCEINLINE bool IsJumping() const { return Jumping; }

	// Getter (Falling)
	virtual bool IsFalling() const override;

	// Getter (Receiving Knockback)
	FORCEINLINE bool IsReceivingKnockback() const { return ReceivingKnockback; }

	bool IsGrounded();

	FHitResult* FindGround(float InDistance);

private:
	bool CanJump();
	void Landed();

	void HandleJumping(float DeltaTime);
	void HandleFalling(float DeltaTime);
	void HandleKnockback(float DeltaTime);

public:
	UPROPERTY()
	FMovementModeUpdated MovementModeUpdated;
	
private:
	EPlatformMovementMode MovementMode = EPlatformMovementMode::Walking;
	
	uint8 CurrentJumpCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping, meta = (AllowPrivateAccess = "true"))
	uint8 MaxJumpsAllowed = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping, meta = (AllowPrivateAccess = "true"))
	int32 MaxJumpHeight = 300;
	
	int32 JumpHeight = MaxJumpHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* JumpCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FallCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* KnockbackCurve = nullptr;

	bool Jumping = false;
	bool Falling = false;
	bool ReceivingKnockback = false;
	
	float CurrentJumpCurveTime = 0.0f;
	float PreviousJumpCurveValue = 0.0f;
	float MaxJumpCurveTime = 0.0f;
	float MinJumpCurveTime = 0.0f;

	float CurrentFallCurveTime = 0.0f;
	float PreviousFallCurveValue = 0.0f;

	FVector KnockbackVector = FVector::ZeroVector;
	float KnockbackVelocity = 0.0f;
	
	float CurrentKnockbackCurveTime = 0.0f;
	float PreviousKnockbackCurveValue = 0.0f;
	float MinKnockbackCurveTime = 0.0f;
	float MaxKnockbackCurveTime = 0.0f;

	UPROPERTY()
	class UBoxComponent* CollisionComponent = nullptr;
};