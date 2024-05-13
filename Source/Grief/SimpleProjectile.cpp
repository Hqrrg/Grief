// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleProjectile.h"

#include "PaperFlipbookComponent.h"
#include "ProjectileManager.h"
#include "SimpleProjectileMovement.h"
#include "Components/BoxComponent.h"
#include "Interfaces/CombatantInterface.h"


#define ENEMY_COLLISION_CHANNEL ECC_GameTraceChannel1

// Sets default values
ASimpleProjectile::ASimpleProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
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
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	
	Hit = false;

	UpdateFlipbook();
}

void ASimpleProjectile::ProjectileHit(AActor* HitActor)
{
	if (!Active) return;
	
	Hit = true;
	UpdateFlipbook();

	SetActorEnableCollision(false);

	bool HitTarget = false;
	
	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(HitActor))
	{
		Combatant->Knockback(GetActorLocation(), KnockbackMultiplier);
		Combatant->Damage(Damage);
		HitTarget = true;
	}
	
	const float CollidedPlaybackLength = FlipbookComponent->GetFlipbookLength();

	if (CollidedPlaybackLength > 0.0f)
	{
		GetWorldTimerManager().SetTimer(CollidedTimerHandle, this, &ASimpleProjectile::Retrieve, CollidedPlaybackLength, false, CollidedPlaybackLength);
	}
	else
	{
		Retrieve();
	}

	Collided(HitTarget);
}

void ASimpleProjectile::FireAt(const AActor* TargetActor)
{
	ResetProjectile();
	
	FVector ProjectileLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector Direction = (TargetLocation - ProjectileLocation).GetSafeNormal();
	
	ProjectileMovement->SetMovementVector(Direction);

	Active = true;
	UpdateFlipbook();

	GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &ASimpleProjectile::Retrieve, Lifetime, 0.0f, Lifetime);

	Fired();
}

void ASimpleProjectile::FireAt(const FVector& TargetLocation)
{
	ResetProjectile();
	
	FVector ProjectileLocation = GetActorLocation();
	FVector Direction = (TargetLocation - ProjectileLocation).GetSafeNormal(); Direction.X = 0.0f;
	
	ProjectileMovement->SetMovementVector(Direction);

	Active = true;
	UpdateFlipbook();

	GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &ASimpleProjectile::Retrieve, Lifetime, false, Lifetime);

	Fired();
}

void ASimpleProjectile::DeflectFrom(const FVector& OriginLocation)
{
	CollisionComponent->SetCollisionResponseToChannel(ENEMY_COLLISION_CHANNEL, ECR_Block);
	
	FVector ProjectileLocation = GetActorLocation();
	FVector Direction = (ProjectileLocation-OriginLocation).GetSafeNormal(); Direction.X = 0.0f;

	FVector TargetLocation = ProjectileLocation + Direction * 1000.0f;
	
	FireAt(TargetLocation);
}

void ASimpleProjectile::Retrieve()
{
	Active = false;
	Hit = false;

	CollisionComponent->SetCollisionProfileName(FName("Projectile"));
	
	bool LifetimeElapsed = false;

	if (GetWorldTimerManager().GetTimerRemaining(LifetimeTimerHandle) <= 0.0f) LifetimeElapsed = true;
	
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (ProjectileManager) ProjectileManager->RetrieveProjectile(this);

	Retrieved(LifetimeElapsed);
}

void ASimpleProjectile::ResetPlatformActor()
{
	Retrieve();
}

void ASimpleProjectile::UpdateFlipbook()
{
	if (!Flipbooks) return;
	
	UPaperFlipbook* Flipbook = nullptr;
	
	if (Active)
	{
		Flipbook = Flipbooks->Default;
	}
	if (Hit)
	{
		Flipbook = Flipbooks->Hit;
	}

	FlipbookComponent->SetFlipbook(Flipbook);
}

