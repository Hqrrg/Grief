// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAreaComponent.h"

#include "Components/BoxComponent.h"
#include "Interfaces/CombatantInterface.h"


// Sets default values for this component's properties
UAttackAreaComponent::UAttackAreaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	ShapeColor = FColor::Red;
}

// Called when the game starts
void UAttackAreaComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UAttackAreaComponent::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UAttackAreaComponent::EndOverlap);
}

void UAttackAreaComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner()) return;
	
	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(OtherActor))
	{
		if (OtherComp != Combatant->GetCollisionComponent()) return;
		ContainedActors.AddUnique(OtherActor);
	}
}

void UAttackAreaComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(OtherActor))
	{
		if (OtherComp != Combatant->GetCollisionComponent()) return;
		ContainedActors.Remove(OtherActor);
	}
}


