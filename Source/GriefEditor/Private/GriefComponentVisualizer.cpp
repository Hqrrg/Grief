// Fill out your copyright notice in the Description page of Project Settings.


#include "GriefComponentVisualizer.h"

#include "Grief/PlayerSensingComponent.h"

void FGriefComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,FPrimitiveDrawInterface* PDI)
{

	if (const UPlayerSensingComponent* PlayerSensingComponent = Cast<UPlayerSensingComponent>(Component))
	{
		if (!PlayerSensingComponent->IsEnabled()) return;
		
		DrawWireSphere(
			PDI,
			PlayerSensingComponent->GetOwner()->GetActorLocation(),
			FColor::Purple,
			PlayerSensingComponent->GetDetectionRadius(),
			24,
			0,
			1.0f,
			0,
			false);


			DrawWireSphere(
		PDI,
		PlayerSensingComponent->GetOwner()->GetActorLocation(),
		FColor::Magenta,
		PlayerSensingComponent->GetSearchRadius(),
		24,
		0,
		1.0f,
		0,
		false);
	}
}
