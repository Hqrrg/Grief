// Fill out your copyright notice in the Description page of Project Settings.


#include "HitboxComponent.h"

#include "Interfaces/EnemyInterface.h"


// Sets default values for this component's properties
UHitboxComponent::UHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	ShapeColor = FColor::Cyan;
}

// Called when the game starts
void UHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UHitboxComponent::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UHitboxComponent::EndOverlap);
}

void UHitboxComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APawn>(OtherActor) && OtherComp->GetClass() != this->GetClass())
	{
		OverlappingEnemies.AddUnique(OtherActor);
	}
}

void UHitboxComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappingEnemies.Contains(OtherActor))
	{
		OverlappingEnemies.Remove(OtherActor);
	}
}


