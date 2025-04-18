﻿// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once

#include "EditorSubsystem.h"
#include "EditorInputCommandsSubsystem.generated.h"

class UEditorInputCommand;
class FUICommandList;

DECLARE_DYNAMIC_DELEGATE(FOnExecute);

UCLASS()
class UEditorInputCommandsSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void OnFilesLoaded();
	void TryRegisterCommands();
	void OnCommandListRegistered(FName CommandListName, TSharedRef<FUICommandList> CommandList);
	
	static void ForeachCommand(const TFunctionRef<void(UEditorInputCommand& Command)>& Func);
};
