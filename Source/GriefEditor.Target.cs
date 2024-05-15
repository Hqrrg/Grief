// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GriefEditorTarget : TargetRules
{
	public GriefEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("GriefEditor");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "GriefEditor"});
	}
}
