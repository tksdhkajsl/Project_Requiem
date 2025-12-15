// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DataInfo : ModuleRules
{
	public DataInfo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine"
        });

        PublicIncludePaths.AddRange(new string[] { "DataInfo/Public" });
        PrivateIncludePaths.AddRange(new string[] { "DataInfo/Private" });
    }
}
