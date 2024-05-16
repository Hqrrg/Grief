// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "SlimeEnemyPawn.generated.h"

UENUM(BlueprintType)
enum class ESlimeAttack : uint8 
{
	Light,
	Explode
};
UCLASS()
class GRIEF_API ASlimeEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateACcess = "true"))
	class UAttackHitboxComponent* LightAttackHitbox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAttackAreaComponent* OutburstAttackArea = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* OutburstNiagara = nullptr;
	
public:
	ASlimeEnemyPawn();

	virtual bool Attack(uint8 AttackID, bool StopMovement) override;

	virtual bool Killed() override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ProxyAttack();

	UFUNCTION()
	void ExplodeAttack();

	virtual void OnAttackFinished(uint8 AttackID) override;

private:
	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetAttackID(ESlimeAttack InSlimeAttack) const { return static_cast<uint8>(InSlimeAttack); }

	FTimerHandle OutburstAttackTimerHandle;
	FTimerHandle KilledTimerHandle;

	FTimerDelegate OutburstAttackTimerDelegate;
	FTimerDelegate KilledTimerDelegate;

	bool CanOutburst = true;
	bool KilledAttackDone = false;
};
