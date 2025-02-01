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
	FARFilter Filter;
	Filter.ClassPaths.Add(UEditorInputCommand::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;

	//copied from UAssetRegistryHelpers::GetBlueprintAssets which is not exposed to other modules
	TArray<FTopLevelAssetPath> BlueprintParentClassPathRoots = MoveTemp(Filter.ClassPaths);
	TSet<FTopLevelAssetPath> BlueprintParentClassPaths;
	if (Filter.bRecursiveClasses)
	{
		AssetRegistry.GetDerivedClassNames(BlueprintParentClassPathRoots, TSet<FTopLevelAssetPath>(),BlueprintParentClassPaths);
	}
	else
	{
		BlueprintParentClassPaths.Append(BlueprintParentClassPathRoots);
	}
	Filter.ClassPaths.Reset(1);
	Filter.ClassPaths.Add(FTopLevelAssetPath(FName(TEXT("/Script/Engine")), FName(TEXT("BlueprintCore"))));
	Filter.bRecursiveClasses = true;

	TArray<FAssetData> AssetDatas;
	auto FilterLambda = [&AssetDatas, &BlueprintParentClassPaths](const FAssetData& AssetData)
	{
		if (BlueprintParentClassPaths.IsEmpty() || UAssetRegistryHelpers::IsAssetDataBlueprintOfClassSet(AssetData, BlueprintParentClassPaths))
		{
			AssetDatas.Add(AssetData);
		}
		return true;
	};
	
	AssetRegistry.EnumerateAssets(Filter, FilterLambda, UE::AssetRegistry::EEnumerateAssetsFlags::None);
	
	for (const FAssetData& AssetData : AssetDatas)
	{
		if (const UBlueprintCore* Blueprint = Cast<UBlueprintCore>(AssetData.GetAsset()))
		{
			if (UEditorInputCommand* Command = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetDefaultObject<UEditorInputCommand>() : nullptr)
			{
				Command->RegisterCommand();
			}	
		}
	}
}

void UCommandExtensionSubsystem::OnFilesLoaded() const
{
	IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();
	AssetRegistry.OnFilesLoaded().RemoveAll(this);
	
	TryRegisterCommands(AssetRegistry);
}
