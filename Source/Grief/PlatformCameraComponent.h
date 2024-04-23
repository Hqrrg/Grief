// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "Enums/Direction.h"
#include "PlatformCameraComponent.generated.h"


USTRUCT(BlueprintType)
struct FCameraBounds
{
	GENERATED_BODY()

	bool UpActive, DownActive, LeftActive, RightActive;
	float Up, Down, Left, Right;
	int32 UpLayer, DownLayer, LeftLayer, RightLayer;

	FCameraBounds()
	{
		UpActive = false;
		DownActive = false;
		LeftActive = false;
		RightActive = false;

		UpLayer = -1;
		DownLayer = -1;
		LeftLayer = -1;
		RightLayer = -1;
		
		Up = FLT_MIN;
		Down = FLT_MAX;
		Left = FLT_MAX;
		Right = FLT_MIN;
	}

	void SetBound(EDirection Direction, const float Value, const int32 LayerID)
	{	
		switch (Direction)
		{
		case EDirection::Up:
			if (Value == Up) break;
			
			Up = Value;
			UpLayer = LayerID;
			UpActive = true;
			break;
			
		case EDirection::Down:
			if (Value == Down) break;
			
			Down = Value;
			DownLayer = LayerID;
			DownActive = true;
			break;
			
		case EDirection::Left:
			if (Value == Left) break;
			
			Left = Value;
			LeftLayer = LayerID;
			LeftActive = true;
			break;
			
		case EDirection::Right:
			if (Value == Right) break;
			
			Right = Value;
			RightLayer = LayerID;
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
