#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttackInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttackInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float KnockbackMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Cooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* Flipbook = nullptr;
	
	bool IsCooldown = false;
};