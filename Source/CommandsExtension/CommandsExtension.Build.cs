// Copyright FifonszGames. All Rights Reserved.

using UnrealBuildTool;

public class CommandsExtension : ModuleRules
{
	public CommandsExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core"
			});
			
		
		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorSubsystem",
				"UnrealEd",
				"Blutility",
				"PropertyEditor",
				"AssetRegistry",
				"BlueprintGraph",
				"GraphEditor",
				"ToolWidgets",
				"Projects",
				"Kismet",
				"InputCore"
			});
	}
}
