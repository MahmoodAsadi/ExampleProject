// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IndependentInputMapperEditor : ModuleRules
{
    public IndependentInputMapperEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "IndependentInputMapper"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "AssetTools",
                "UnrealEd",
                "PropertyEditor",
                "ToolMenus",
                "Settings"
            }
        );
    }
}