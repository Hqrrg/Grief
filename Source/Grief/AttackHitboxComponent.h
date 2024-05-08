// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces\PlatformActorInterface.h"
#include "AttackHitboxComponent.generated.h"


enum class EDirection : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIEF_API UAttackHitboxComponent : public UBoxComponent, public IPlatformActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttackHitboxComponent();

protected:
	virtual void BeginPlay() override;;

public:
	FORCEINLINE TArray<AActor*> GetContainedActors() const { return ContainedActors; }
	
private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY()
	TArray<AActor*> ContainedActors;

public:
	virtual void ResetPlatformActor() override;
};