// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionSubsystem.h"

#include "EditorInputCommand.h"
#include "AssetRegistry/AssetRegistryHelpers.h"
#include "AssetRegistry/IAssetRegistry.h"


bool UCommandExtensionSubsystem::MapAction(const TSharedRef<FUICommandList>& List, const TSharedRef<FUICommandInfo>& CommandInfo, const FOnExecute& Func, EUIActionRepeatMode RepeatMode)
{
	FOnExecuteMulticast* Multicast = BindMap.Find(List);
	if (!Multicast)
	{
		Multicast = &BindMap.Add(List);
		List->MapAction(CommandInfo, FExecuteAction::CreateWeakLambda(this, [Multicast]
		{
			if (Multicast)
			{
				Multicast->Broadcast();
			}
		}), RepeatMode);
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
		TryRegisterCommands(AssetRegistry);
	}
}

void UCommandExtensionSubsystem::TryRegisterCommands(const IAssetRegistry& AssetRegistry)
{
	TArray<FAssetData> InputCommands;
	AssetRegistry.GetAssetsByClass(UEditorInputCommand::StaticClass()->GetClassPathName(), InputCommands);
	for (const FAssetData& AssetData : InputCommands)
	{
		if (UEditorInputCommand* Command = Cast<UEditorInputCommand>(AssetData.GetAsset()))
		{
			Command->RegisterCommand();
		}
	}
}

void UCommandExtensionSubsystem::OnFilesLoaded() const
{
	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	AssetRegistry.OnFilesLoaded().RemoveAll(this);
	
	TryRegisterCommands(AssetRegistry);
}
