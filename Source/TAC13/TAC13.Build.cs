// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TAC13 : ModuleRules
{
	public TAC13(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { "TAC13" });
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"UMG", 
			"NavigationSystem", 
			"AIModule", 
			"GameplayTasks",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "AnimGraphRuntime" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "TACPCH.h";
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bUseUnity = false;
	}
}
