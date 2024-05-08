// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Interfaces\PlatformActorInterface.h"
#include "SimpleProjectile.generated.h"


USTRUCT(BlueprintType)
struct FProjectileFlipbooks : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UPaperFlipbook* Default = nullptr;

	UPROPERTY(EditAnywhere)
	class UPaperFlipbook* Hit = nullptr;
};

UCLASS()
class GRIEF_API ASimpleProjectile : public AActor, public IPlatformActorInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* FlipbookComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USimpleProjectileMovement* ProjectileMovement = nullptr;

public:
	// Sets default values for this actor's properties
	ASimpleProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Fired();

	UFUNCTION(BlueprintImplementableEvent)
	void Collided(bool HitTarget);

	UFUNCTION(BlueprintImplementableEvent)
	void Retrieved(bool LifetimeElapsed);

public:
	void ResetProjectile();
	
public:
	UFUNCTION()
	virtual void ProjectileHit(AActor* HitActor);
	
	void FireAt(const AActor* TargetActor);
	void FireAt(const FVector& TargetLocation);

	void DeflectFrom(const FVector& OriginLocation);

private:
	void Retrieve();

public:
	FORCEINLINE bool IsDeflectable() const { return Deflectable && !Hit; }

public:
	FORCEINLINE void SetProjectileManager(class AProjectileManager* InProjectileManager) { ProjectileManager = InProjectileManager; }
	
	FORCEINLINE void SetAttackValues(float InDamage, float InKnockbackMultiplier) { Damage = InDamage; KnockbackMultiplier = InKnockbackMultiplier; };
	FORCEINLINE void GetAttackValues(float& InDamage, float &InKnockbackMultiplier) { InDamage = Damage; InKnockbackMultiplier = KnockbackMultiplier; };

public:
	virtual void ResetPlatformActor() override;

private:
	void UpdateFlipbook();
	
private:
	FProjectileFlipbooks* Flipbooks = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	FName FlipbookDataTableKey = FName("Default");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance, meta = (AllowPrivateAccess = "true"))
	UDataTable* FlipbookDataTable = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Manager", meta = (AllowPrivateAccess = "true"))
	float Lifetime = 5.0f;

	FTimerHandle LifetimeTimerHandle;
	FTimerHandle CollidedTimerHandle;
	
	UPROPERTY()
	class AProjectileManager* ProjectileManager = nullptr;

	float Damage = 0.0f;
	float KnockbackMultiplier = 0.0f;
	
	bool Active = true;
	bool Hit = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Projectile, meta = (AllowPrivateAccess = "true"))
	bool Deflectable = true;
	
};
