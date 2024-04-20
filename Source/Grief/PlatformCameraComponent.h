// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GriefCharacter.h"
#include "Components/ActorComponent.h"
#include "PlatformCameraComponent.generated.h"


enum class EDirection : uint8;

USTRUCT(BlueprintType)
struct FCameraBounds
{
	GENERATED_BODY()

	bool UpActive, DownActive, LeftActive, RightActive;
	float Up, Down, Left, Right;

	FCameraBounds()
	{
		UpActive = false;
		DownActive = false;
		LeftActive = false;
		RightActive = false;
		
		Up = FLT_MIN;
		Down = FLT_MAX;
		Left = FLT_MAX;
		Right = FLT_MIN;
	}

	void SetBound(EDirection Direction, const float Value)
	{	
		switch (Direction)
		{
		case EDirection::Up:
			if (Value == Up) break;
			
			Up = Value;
			UpActive = true;
			break;
			
		case EDirection::Down:
			if (Value == Down) break;
			
			Down = Value;
			DownActive = true;
			break;
			
		case EDirection::Left:
			if (Value == Left) break;
			
			Left = Value;
			LeftActive = true;
			break;
			
		case EDirection::Right:
			if (Value == Right) break;
			
			Right = Value;
			RightActive = true;
			break;
			
		default:
			break;
		}
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIEF_API UPlatformCameraComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera = nullptr;

public:
	// Sets default values for this component's properties
	UPlatformCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void AddCameraBoundingBox(class ACameraBoundingBox* CameraBoundingBox);
	void RemoveCameraBoundingBox(class ACameraBoundingBox* CameraBoundingBox);
	
private:
	void SetupCamera();

	UFUNCTION(BlueprintPure)
	FORCEINLINE FCameraBounds GetCameraBounds() const { return CameraBounds; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float DistanceToCharacter = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float YawMovementBias = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float PitchMovementBias = 10.0f;

	UPROPERTY()
	TArray<class ACameraBoundingBox*> CameraBoundingBoxes;

	UPROPERTY(EditDefaultsOnly)
	FCameraBounds CameraBounds;

	void UpdateCameraBounds();
};
