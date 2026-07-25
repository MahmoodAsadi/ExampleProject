// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IndependentInputManagerEditor : ModuleRules
{
    public IndependentInputManagerEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "IndependentInputManager"
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
                "UnrealEd",
                "PropertyEditor",
                "Settings",
                "ToolMenus",
                "Projects"
            }
        );
    }
}