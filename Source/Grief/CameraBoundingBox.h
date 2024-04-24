// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraBoundingBox.generated.h"

enum class EDirection : uint8;

UCLASS()
class GRIEF_API ACameraBoundingBox : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoundingBox = nullptr;

public:
	// Sets default values for this actor's properties
	ACameraBoundingBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	float GetBound(EDirection Direction);

	bool IsBoundActive(EDirection Direction);
	
	FORCEINLINE int32 GetLayer() const { return Layer; }
	
private:
	int32 Layer = 0;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Options|Bounds", meta = (AllowPrivateAccess = "true"))
	bool Up = true;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Options|Bounds", meta = (AllowPrivateAccess = "true"))
	bool Down = true;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Options|Bounds", meta = (AllowPrivateAccess = "true"))
	bool Left = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Options|Bounds", meta = (AllowPrivateAccess = "true"))
	bool Right = true;
};
