// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "CommandExtensionSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnExecute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecuteMulticast);

UCLASS()
class UCommandExtensionSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	bool MapAction(const TSharedRef<FUICommandList>& List, const TSharedRef<FUICommandInfo>& CommandInfo, const FOnExecute& Func,
		EUIActionRepeatMode RepeatMode = EUIActionRepeatMode::RepeatDisabled);
	
	bool UnMapAction(const TSharedRef<FUICommandList>& List, const TSharedRef<FUICommandInfo>& CommandInfo, const FOnExecute& Func);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void OnFilesLoaded() const;
	static void TryRegisterCommands(const IAssetRegistry& AssetRegistry);

	TMap<TWeakPtr<FUICommandList>, FOnExecuteMulticast> BindMap;
};
