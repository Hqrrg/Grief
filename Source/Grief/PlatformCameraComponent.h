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
	
	float Up, Down, Left, Right;
	int32 LayerUp, LayerDown, LayerLeft, LayerRight;

	FCameraBounds()
	{
		LayerUp = INT_MIN;
		LayerDown = INT_MIN;
		LayerLeft = INT_MIN;
		LayerRight = INT_MIN;
		
		Up = FLT_MAX;
		Down = FLT_MIN;
		Left = FLT_MIN;
		Right = FLT_MAX;
	}

	void SetBound(EDirection Direction, const float Bound, const int32 LayerID)
	{	
		switch (Direction)
		{
		case EDirection::Up:
			Up = Bound;
			LayerUp = LayerID;
			break;
			
		case EDirection::Down:
			Down = Bound;
			LayerDown = LayerID;
			break;
			
		case EDirection::Left:
			Left = Bound;
			LayerLeft = LayerID;
			break;
			
		case EDirection::Right:
			Right = Bound;
			LayerRight = LayerID;
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
	void UpdateBound(float &Bound, const float InBound, int32 &Layer, const int32 InLayer);
};
