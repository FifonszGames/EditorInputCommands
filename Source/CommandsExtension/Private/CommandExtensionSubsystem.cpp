// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionSubsystem.h"

#include "CommandsExtension.h"
#include "EditorInputCommand.h"
#include "AssetRegistry/IAssetRegistry.h"

void UCommandExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddUObject(this, &UCommandExtensionSubsystem::OnFilesLoaded);
	}
	else
	{
		TryRegisterCommands();
	}
}

void UCommandExtensionSubsystem::Deinitialize()
{
	FInputBindingManager::Get().OnRegisterCommandList.RemoveAll(this);
	Super::Deinitialize();
}

void UCommandExtensionSubsystem::TryRegisterCommands()
{
	FCommandsExtensionModule& Module = FModuleManager::GetModuleChecked<FCommandsExtensionModule>(FCommandsExtensionModule::GetModuleName());
	Module.TryAddUnregisteredLists();
	
	ForeachCommand([](UEditorInputCommand& Command){ Command.RegisterCommand(); });
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.AddUObject(this, &UCommandExtensionSubsystem::OnCommandListRegistered);
}

void UCommandExtensionSubsystem::OnCommandListRegistered(FName CommandListName, TSharedRef<FUICommandList> CommandList)
{
	ForeachCommand([&CommandListName](UEditorInputCommand& Command)
	{
		if (Command.GetCommandListIdentifier() == CommandListName)
		{
			//this will do a lot more work than needed make it so that it only adds binding for the new list if necessary
			Command.MapToTargetList();
		}
	});
}

void UCommandExtensionSubsystem::ForeachCommand(const TFunctionRef<void(UEditorInputCommand& Command)>& Func)
{
	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	
	TArray<FAssetData> InputCommands;
	AssetRegistry.GetAssetsByClass(UEditorInputCommand::StaticClass()->GetClassPathName(), InputCommands);
	for (const FAssetData& AssetData : InputCommands)
	{
		if (UEditorInputCommand* Command = Cast<UEditorInputCommand>(AssetData.GetAsset()))
		{
			Func(*Command);
		}
	}
}

void UCommandExtensionSubsystem::OnFilesLoaded()
{
	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	AssetRegistry.OnFilesLoaded().RemoveAll(this);
	TryRegisterCommands();
}
