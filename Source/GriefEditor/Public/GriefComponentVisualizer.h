// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR
#include "ComponentVisualizer.h"

class FGriefComponentVisualizer : public FComponentVisualizer
{
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};

#endif