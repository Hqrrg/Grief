#include "GriefEditor.h"

#if WITH_EDITOR
#include "GriefComponentVisualizer.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#endif

#include "..\..\Grief\PlayerSensingComponent.h"

#define LOCTEXT_NAMESPACE "FGriefEditorModule"

void FGriefEditorModule::StartupModule()
{
#if WITH_EDITOR
	if (GUnrealEd)
	{
		TSharedPtr<FGriefComponentVisualizer> Visualizer = MakeShareable(new FGriefComponentVisualizer());
		GUnrealEd->RegisterComponentVisualizer(UPlayerSensingComponent::StaticClass()->GetFName(), Visualizer);
		Visualizer->OnRegister();
	}
#endif
    
}

void FGriefEditorModule::ShutdownModule()
{
#if WITH_EDITOR
    if (GUnrealEd)
    {
	    GUnrealEd->UnregisterComponentVisualizer(UPlayerSensingComponent::StaticClass()->GetFName());
    }
#endif
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGriefEditorModule, GriefEditor)