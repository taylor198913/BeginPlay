// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BeginPlay : ModuleRules
{
	public BeginPlay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
