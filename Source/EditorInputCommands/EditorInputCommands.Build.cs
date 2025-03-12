// Copyright FifonszGames. All Rights Reserved.

using UnrealBuildTool;

public class EditorInputCommands : ModuleRules
{
	public EditorInputCommands(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"Slate"
			});
		
		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"CoreUObject",
				"Engine",
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
