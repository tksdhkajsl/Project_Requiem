// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_Requiem : ModuleRules
{
	public Project_Requiem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
        "DataInfo", "ComponentSystems"});
	}
}
