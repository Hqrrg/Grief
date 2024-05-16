// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolRoute.h"

AAIPatrolRoute::AAIPatrolRoute()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
}

FVector AAIPatrolRoute::GetLocationAtIndex(uint8 Index)
{
	const FVector ActorLocation = GetActorLocation();
	return ActorLocation + Route[Index];
}
