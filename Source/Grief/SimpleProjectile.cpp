// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleProjectile.h"

#include "PaperFlipbookComponent.h"
#include "ProjectileManager.h"
#include "SimpleProjectileMovement.h"
#include "Components/BoxComponent.h"
#include "Interfaces/CombatantInterface.h"


// Sets default values
ASimpleProjectile::ASimpleProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionProfileName(FName("Projectile"));
	CollisionComponent->SetHiddenInGame(false);
	SetRootComponent(CollisionComponent);

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	FlipbookComponent->SetupAttachment(CollisionComponent);
	
	ProjectileMovement = CreateDefaultSubobject<USimpleProjectileMovement>(TEXT("ProjectileMovement"));
	AddOwnedComponent(ProjectileMovement);
}

// Called when the game starts or when spawned
void ASimpleProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	ProjectileMovement->OnProjectileHit.AddDynamic(this, &ASimpleProjectile::ProjectileHit);
	
	if (FlipbookDataTable)
	{
		static const FString ContextString(FString::Printf(TEXT("%s Projectile Flipbooks Context"), *FlipbookDataTableKey.ToString()));
		Flipbooks = FlipbookDataTable->FindRow<FProjectileFlipbooks>(FlipbookDataTableKey, ContextString, true);
	}
}

void ASimpleProjectile::ResetProjectile()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	Moving = false;
	Hit = false;

	UpdateFlipbook();
}

void ASimpleProjectile::ProjectileHit(AActor* HitActor)
{
	Hit = true;
	UpdateFlipbook();

	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(HitActor))
	{
		Combatant->Knockback(GetActorLocation(), KnockbackMultiplier);
		Combatant->ApplyDamage(Damage);
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (ProjectileManager) ProjectileManager->RetrieveProjectile(this);
}

void ASimpleProjectile::FireAt(const AActor* TargetActor)
{
	ResetProjectile();
	
	FVector ProjectileLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector Direction = (TargetLocation - ProjectileLocation).GetSafeNormal();
	
	ProjectileMovement->SetMovementVector(Direction);

	Moving = true;
	UpdateFlipbook();
}

void ASimpleProjectile::FireAt(const FVector& TargetLocation)
{
	ResetProjectile();
	
	FVector ProjectileLocation = GetActorLocation();
	FVector Direction = (TargetLocation - ProjectileLocation).GetSafeNormal();
	
	ProjectileMovement->SetMovementVector(Direction);

	Moving = true;
	UpdateFlipbook();
}

void ASimpleProjectile::UpdateFlipbook()
{
	if (!Flipbooks) return;
	
	UPaperFlipbook* Flipbook = nullptr;
	
	if (Moving)
	{
		Flipbook = Flipbooks->Moving;
	}
	if (Hit)
	{
		Flipbook = Flipbooks->Hit;
	}

	FlipbookComponent->SetFlipbook(Flipbook);
}

