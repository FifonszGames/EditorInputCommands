// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "CommandExtensionSubsystem.generated.h"

class UEditorInputCommand;

DECLARE_DYNAMIC_DELEGATE(FOnExecute);

UCLASS()
class UCommandExtensionSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void OnActionExecuted(FOnExecute OnExecute);
	
	void OnFilesLoaded();
	void TryRegisterCommands();
	void OnCommandListRegistered(FName CommandListName, TSharedRef<FUICommandList> CommandList);
	
	static void ForeachCommand(const TFunctionRef<void(UEditorInputCommand& Command)>& Func);
};
