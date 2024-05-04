// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPawn.h"

#include "Components/BoxComponent.h"


// Sets default values
ABossPawn::ABossPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent->SetCollisionProfileName(FName("Boss"));
	
	MaxHealth = 18.0f;
}

