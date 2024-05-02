// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossPawn.h"
#include "AngerBossPawn.generated.h"

UENUM(BlueprintType)
enum class EAngerBossAttack : uint8
{
	Fireball,
	Beam,
	Outburst
};

UCLASS()
class GRIEF_API AAngerBossPawn : public ABossPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* BeamRail = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAttackAreaComponent* OutburstAttackArea = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* OutburstNiagara = nullptr;

public:
	// Sets default values for this pawn's properties
	AAngerBossPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual bool Attack(uint8 AttackID) override;

private:
	UFUNCTION()
	void Attack_Fireball();

	UFUNCTION()
	void Attack_Beam();

	UFUNCTION()
	void Attack_Outburst();

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(EAngerBossAttack InAngerBossAttack) const { return static_cast<uint8>(InAngerBossAttack); }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Beam", meta = (AllowPrivateAccess = "true"))
	float BeamRailRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Beam", meta = (AllowPrivateAccess = "true"))
	int32 BeamCount = 4;
	
private:
	FTimerHandle FireballAttackTimerHandle;
	FTimerHandle BeamAttackTimerHandle;
	FTimerHandle OutburstAttackTimerHandle;

	FTimerDelegate FireballAttackTimerDelegate;
	FTimerDelegate BeamAttackTimerDelegate;
	FTimerDelegate OutburstAttackTimerDelegate;

	bool CanOutburst = true;
	bool CanFireball = true;
};
