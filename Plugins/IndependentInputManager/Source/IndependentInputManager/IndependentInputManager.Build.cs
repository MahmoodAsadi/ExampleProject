// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class IndependentInputManager : ModuleRules
{
    public IndependentInputManager(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "InputDevice",
                "ApplicationCore",
                "DeveloperSettings"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
            }
            );

        // SDL3 path setup
        string SDLPath = Path.Combine(ModuleDirectory, "../ThirdParty/SDL3");

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(SDLPath, "Include"));

            // Library path
            string LibPath = Path.Combine(SDLPath, "Lib/Win64");

            // Import library (required at link time)
            PublicAdditionalLibraries.Add(Path.Combine(LibPath, "SDL3.lib"));

            // Runtime DLL
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/SDL3/Lib/Win64/SDL3.dll");

            PublicDelayLoadDLLs.Add("SDL3.dll");

            // Windows HID support (Adaptive Triggers, LEDs, etc.)
            PublicSystemLibraries.Add("Setupapi.lib");
            PublicSystemLibraries.Add("Hid.lib");
        }
    }
}
