// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DeathMatch : ModuleRules
{
	public DeathMatch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		// ���� ����� Module�� Ȱ��ȭ �ϱ� ���� �߰�
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystemSteam", "OnlineSubsystem" , "Niagara", "PhysicsCore", "NavigationSystem", "AIModule" , "GameplayTasks", "AnimGraphRuntime" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
