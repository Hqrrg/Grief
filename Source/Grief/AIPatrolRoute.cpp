// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolRoute.h"

FVector AAIPatrolRoute::GetLocationAtIndex(uint8 Index)
{
	const FVector ActorLocation = GetActorLocation();
	return ActorLocation + Route[Index];
}
