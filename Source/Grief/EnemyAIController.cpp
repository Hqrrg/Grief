// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Interfaces/EnemyInterface.h"


// Sets default values
AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(InPawn))
	{
		UBehaviorTree* BehaviorTree = Enemy->GetBehaviourTree();
		RunBehaviorTree(BehaviorTree);
	}
}
