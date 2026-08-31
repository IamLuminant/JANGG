// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JANGG : ModuleRules
{
	public JANGG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"JANGG",
			"JANGG/Variant_Platforming",
			"JANGG/Variant_Platforming/Animation",
			"JANGG/Variant_Combat",
			"JANGG/Variant_Combat/AI",
			"JANGG/Variant_Combat/Animation",
			"JANGG/Variant_Combat/Gameplay",
			"JANGG/Variant_Combat/Interfaces",
			"JANGG/Variant_Combat/UI",
			"JANGG/Variant_SideScrolling",
			"JANGG/Variant_SideScrolling/AI",
			"JANGG/Variant_SideScrolling/Gameplay",
			"JANGG/Variant_SideScrolling/Interfaces",
			"JANGG/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
