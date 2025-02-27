// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "CommandExtensionSubsystem.generated.h"

class UEditorInputCommand;

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
	void OnFilesLoaded();
	void TryRegisterCommands();
	static void ForeachCommand(const TFunctionRef<void(UEditorInputCommand& Command)>& Func);

	TMap<TWeakPtr<FUICommandList>, FOnExecuteMulticast> BindMap;
	bool bCommandsRegistered = false;
};
