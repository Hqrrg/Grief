// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlatformCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIEF_API UPlatformCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlatformCharacterMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual bool DoJump(bool bReplayingMoves) override;

	virtual bool IsFalling() const override;

private:
	bool HasFoundFloor(FFindFloorResult& FindFloorResult, const FVector TraceStart, const FVector TraceEnd) const;

	void HandleJumping(const float DeltaTime);

	void HandleFalling(const float DeltaTime);

	void SetFalling();
	
private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* JumpCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FallCurve = nullptr;

	bool Jumping = false;
	float JumpCurveTime = 0.0f;
	float LastJumpCurveValue = 0.0f;
	float JumpCurveMin = 0.0f;
	float JumpCurveMax = 0.0f;

	bool Falling = false;
	float FallCurveTime = 0.0f;
	float LastFallCurveValue = 0.0f;
};
