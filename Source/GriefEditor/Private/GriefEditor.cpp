#include "GriefEditor.h"

#include "GriefComponentVisualizer.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "..\..\Grief\PlayerSensingComponent.h"

#define LOCTEXT_NAMESPACE "FGriefEditorModule"

void FGriefEditorModule::StartupModule()
{
	if (GUnrealEd)
	{
		TSharedPtr<FGriefComponentVisualizer> Visualizer = MakeShareable(new FGriefComponentVisualizer());
		GUnrealEd->RegisterComponentVisualizer(UPlayerSensingComponent::StaticClass()->GetFName(), Visualizer);
		Visualizer->OnRegister();
	}
    
}

void FGriefEditorModule::ShutdownModule()
{
    if (GUnrealEd)
    {
	    GUnrealEd->UnregisterComponentVisualizer(UPlayerSensingComponent::StaticClass()->GetFName());
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGriefEditorModule, GriefEditor)