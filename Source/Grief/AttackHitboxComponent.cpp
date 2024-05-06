// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitboxComponent.h"

#include "SimpleProjectile.h"
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
		if (OtherComp != Combatant->GetCollisionComponent()) return;
		ContainedActors.AddUnique(OtherActor);
	}

	if (ASimpleProjectile* Projectile = Cast<ASimpleProjectile>(OtherActor))
	{
		ContainedActors.AddUnique(Projectile);
	}
}

void UAttackHitboxComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(OtherActor))
	{
		if (OtherComp != Combatant->GetCollisionComponent()) return;
		ContainedActors.Remove(OtherActor);
	}

	if (ASimpleProjectile* Projectile = Cast<ASimpleProjectile>(OtherActor))
	{
		ContainedActors.Remove(Projectile);
	}
}


