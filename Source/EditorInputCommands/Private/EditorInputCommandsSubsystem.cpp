// Copyright FifonszGames 2025 All Rights Reserved.


#include "EditorInputCommandsSubsystem.h"

#include "EditorInputCommands.h"
#include "EditorInputCommand.h"
#include "AssetRegistry/IAssetRegistry.h"

void UEditorInputCommandsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddUObject(this, &UEditorInputCommandsSubsystem::OnFilesLoaded);
	}
	else
	{
		TryRegisterCommands();
	}
}

void UEditorInputCommandsSubsystem::Deinitialize()
{
	FInputBindingManager::Get().OnRegisterCommandList.RemoveAll(this);
	Super::Deinitialize();
}

void UEditorInputCommandsSubsystem::TryRegisterCommands()
{
	FEditorInputCommandsModule& Module = FModuleManager::GetModuleChecked<FEditorInputCommandsModule>(FEditorInputCommandsModule::GetModuleName());
	Module.TryAddUnregisteredLists();
	
	ForeachCommand([](UEditorInputCommand& Command){ Command.RegisterCommand(); });
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.AddUObject(this, &UEditorInputCommandsSubsystem::OnCommandListRegistered);
}

void UEditorInputCommandsSubsystem::OnCommandListRegistered(FName CommandListName, TSharedRef<FUICommandList> CommandList)
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

void UEditorInputCommandsSubsystem::ForeachCommand(const TFunctionRef<void(UEditorInputCommand& Command)>& Func)
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

void UEditorInputCommandsSubsystem::OnFilesLoaded()
{
	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	AssetRegistry.OnFilesLoaded().RemoveAll(this);
	TryRegisterCommands();
}
