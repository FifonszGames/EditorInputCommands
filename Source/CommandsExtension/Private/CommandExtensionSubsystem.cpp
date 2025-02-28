// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionSubsystem.h"

#include "EditorInputCommand.h"
#include "AssetRegistry/IAssetRegistry.h"


bool UCommandExtensionSubsystem::MapAction(const TSharedRef<FUICommandList>& List, const TSharedRef<FUICommandInfo>& CommandInfo, const FOnExecute& Func, EUIActionRepeatMode RepeatMode)
{
	FOnExecuteMulticast* Multicast = BindMap.Find(List);
	if (!Multicast)
	{
		//TODO:: this will make it so that any command bound to the same list will always get executed no matter the input chors since we bind those to the same multicast !!
		Multicast = &BindMap.Add(List);
		if (!List->IsActionMapped(CommandInfo))
		{
			List->MapAction(CommandInfo, FExecuteAction::CreateUObject(this, &UCommandExtensionSubsystem::OnCommandExecuted, Multicast), RepeatMode);
		}
	}
	Multicast->AddUnique(Func);
	return true;
}

bool UCommandExtensionSubsystem::UnMapAction(const TSharedRef<FUICommandList>& List, const TSharedRef<FUICommandInfo>& CommandInfo, const FOnExecute& Func)
{
	if (FOnExecuteMulticast* Multicast = BindMap.Find(List))
	{
		if (Multicast->Contains(Func))
		{
			List->UnmapAction(CommandInfo);
			Multicast->Remove(Func);
			if (!Multicast->IsBound())
			{
				BindMap.Remove(List);
			}
			return true;
		}
	}
	return false;
}

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
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.RemoveAll(this);
	Super::Deinitialize();
}

void UCommandExtensionSubsystem::TryRegisterCommands()
{
	ForeachCommand([](UEditorInputCommand& Command){ Command.RegisterCommand(); });
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.AddUObject(this, &UCommandExtensionSubsystem::OnCommandListRegistered);
}

void UCommandExtensionSubsystem::OnCommandListRegistered(FName CommandListName, TSharedRef<FUICommandList> CommandList)
{
	TArray<TWeakPtr<FUICommandList>> Keys;
	BindMap.GenerateKeyArray(Keys);
	for (const TWeakPtr<FUICommandList>& Key : Keys)
	{
		if(!Key.IsValid())
		{
			BindMap.Remove(Key);
		}
	} 
	ForeachCommand([&CommandListName](UEditorInputCommand& Command)
	{
		if (Command.GetCommandListIdentifier() == CommandListName)
		{
			//TODO:: this will do a lot more work than needed, fix so that it only adds binding for new list
			Command.MapToTargetList();
		}
	});
}

void UCommandExtensionSubsystem::OnCommandExecuted(FOnExecuteMulticast* Multicast)
{
	if (Multicast)
	{
		Multicast->Broadcast();
	}	
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
