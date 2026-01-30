// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ButWhy : ModuleRules
{
	public ButWhy(ReadOnlyTargetRules Target) : base(Target)
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
			"ButWhy",
			"ButWhy/Variant_Platforming",
			"ButWhy/Variant_Platforming/Animation",
			"ButWhy/Variant_Combat",
			"ButWhy/Variant_Combat/AI",
			"ButWhy/Variant_Combat/Animation",
			"ButWhy/Variant_Combat/Gameplay",
			"ButWhy/Variant_Combat/Interfaces",
			"ButWhy/Variant_Combat/UI",
			"ButWhy/Variant_SideScrolling",
			"ButWhy/Variant_SideScrolling/AI",
			"ButWhy/Variant_SideScrolling/Gameplay",
			"ButWhy/Variant_SideScrolling/Interfaces",
			"ButWhy/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
