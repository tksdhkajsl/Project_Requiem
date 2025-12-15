// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ComponentSystems : ModuleRules
{
	public ComponentSystems(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "DeveloperSettings",
        "DataInfo"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });


        PublicIncludePaths.AddRange(new string[] { "ComponentSystems/Public" });
        PrivateIncludePaths.AddRange(new string[] { "ComponentSystems/Private" });
    }
}
