// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitboxComponent.h"

#include "Interfaces/CombatantInterface.h"


// Sets default values for this component's properties
UAttackHitboxComponent::UAttackHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	ShapeColor = FColor::Red;
}

// Called when the game starts
void UAttackHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UAttackHitboxComponent::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UAttackHitboxComponent::EndOverlap);
}

void UAttackHitboxComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner()) return;
	
	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(OtherActor))
	{
		FName HitboxCollisionName = Combatant->GetCollisionComponent()->GetCollisionProfileName();
		
		if (OtherComp->GetCollisionProfileName() == HitboxCollisionName)
		{
			ContainedActors.AddUnique(OtherActor);
		}
	}
}

void UAttackHitboxComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(OtherActor))
	{
		FName HitboxCollisionName = Combatant->GetCollisionComponent()->GetCollisionProfileName();
		
		if (OtherComp->GetCollisionProfileName() == HitboxCollisionName)
		{
			ContainedActors.Remove(OtherActor);
		}
	}
}


