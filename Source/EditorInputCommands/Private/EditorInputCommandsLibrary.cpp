// Copyright FifonszGames. All Rights Reserved.

#include "EditorInputCommandsLibrary.h"

#include "EditorInputCommands.h"
#include "EditorInputCommand.h"

namespace CMDUtils
{
	EUIActionRepeatMode Convert(ECommandRepeatMode Mode)
	{
		return Mode == ECommandRepeatMode::Enabled ? EUIActionRepeatMode::RepeatEnabled : EUIActionRepeatMode::RepeatDisabled;
	}
}

void UEditorInputCommandsLibrary::RegisterInputCommand(const FInputCommandRegisterData& InData, ERegistrationResult& OutResult)
{
	if (!InData.IsValid())
	{
		OutResult = ERegistrationResult::InvalidRegistrationData;
		return;
	}
	
	const FCommandIdentifier Identifier = InData.GetIdentifier();
	if (Identifier.IsRegistered())
	{
		OutResult = ERegistrationResult::AlreadyRegistered;
		return;
	}
	
	TSharedPtr<FUICommandInfo> Info;
	FUICommandInfo::MakeCommandInfo(
		InData.GetContext().ToSharedRef(), 
		Info,
		InData.Identifier,
		InData.Label,
		InData.Description,
		InData.GetIcon(),
		EUserInterfaceActionType::None,
		InData.DefaultChord,
		InData.AlternateDefaultChord);

	OutResult = ERegistrationResult::Success;
}

bool UEditorInputCommandsLibrary::UnregisterInputCommand(const FCommandIdentifier& CommandIdentifier)
{
	TSharedPtr<FBindingContext> BindingContext = CommandIdentifier.AsContext();
	if (!BindingContext.IsValid())
	{
		return false;
	}
	TSharedPtr<FUICommandInfo> Info = CommandIdentifier.AsInfo();
	if (!Info.IsValid())
	{
		return false;
	}
	FInputBindingManager::Get().RemoveInputCommand(BindingContext.ToSharedRef(), Info.ToSharedRef());
	return true;
}

bool UEditorInputCommandsLibrary::MapAction(const FCommandMappingData& InMappingData, FOnExecute OnExecute, const bool bInOverrideIfAlreadyMapped, ECommandRepeatMode RepeatMode)
{
	TSharedPtr<FUICommandInfo> CommandInfo = InMappingData.CommandIdentifier.AsInfo();
	if (CommandInfo.IsValid())
	{
		const EUIActionRepeatMode Mode = CMDUtils::Convert(RepeatMode);
		return InMappingData.TargetList.ForeachCommandList([&CommandInfo, &OnExecute, Mode, bInOverrideIfAlreadyMapped](const TSharedRef<FUICommandList>& List)
		{
			if (bInOverrideIfAlreadyMapped || !List->IsActionMapped(CommandInfo))
			{
				List->MapAction(CommandInfo, FExecuteAction::CreateLambda([OnExecute](){ OnExecute.ExecuteIfBound(); }), Mode);
				return true;
			}
			return false;
		});
	}
	return false;
}

bool UEditorInputCommandsLibrary::MapActionToCommand(UEditorInputCommand* CommandAsset, const FOnExecute& OnExecute)
{
	if (CommandAsset && OnExecute.IsBound())
	{
		CommandAsset->OnInputCommandExecuted.AddUnique(OnExecute);
		return true;
	}
	return false;
}

bool UEditorInputCommandsLibrary::UnmapAction(const FCommandMappingData& InMappingData)
{
	TSharedPtr<FUICommandInfo> CommandInfo = InMappingData.CommandIdentifier.AsInfo();
	if (CommandInfo.IsValid())
	{
		InMappingData.TargetList.ForeachCommandList([&CommandInfo](const TSharedRef<FUICommandList>& List)
		{
			List->UnmapAction(CommandInfo);
			return true;
		});
	}
	return false;
}

bool UEditorInputCommandsLibrary::UnmapActionFromCommand(UEditorInputCommand* CommandAsset, const FOnExecute& OnExecute)
{
	if (CommandAsset && CommandAsset->OnInputCommandExecuted.Contains(OnExecute))
	{
		CommandAsset->OnInputCommandExecuted.Remove(OnExecute);
		return true;
	}
	return false;
}

bool UEditorInputCommandsLibrary::GetUserDefinedCommandInputChord(const FCommandIdentifier& CommandIdentifier, const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord)
{
	return CommandIdentifier.GetUserDefinedChord(InChordIndex, OutChord);
}

bool UEditorInputCommandsLibrary::IsCommandRegistered(const FCommandIdentifier& CommandIdentifier)
{
	return CommandIdentifier.IsRegistered();
}

FCommandIdentifier UEditorInputCommandsLibrary::MakeCommandIdentifier(const FName BindingContext, const FName Identifier)
{
	return FCommandIdentifier(BindingContext, Identifier);
}

FExistingContextBinding UEditorInputCommandsLibrary::MakeExistingContextBinding(const FName BindingContextName)
{
	return FExistingContextBinding(BindingContextName);
}

FCommandListIdentifier UEditorInputCommandsLibrary::MakeCommandListIdentifier(const FName ListIdentifier)
{
	return FCommandListIdentifier(ListIdentifier);
}

TArray<FName> UEditorInputCommandsLibrary::GetBindingContextNames()
{
	TArray<FName> Names;
	ForeachBindingContext([&Names](const TSharedPtr<FBindingContext>& Context)
	{
		Names.AddUnique(Context->GetContextName());
	});
	return Names;
}

void UEditorInputCommandsLibrary::ForeachBindingContext(const TFunctionRef<void(const TSharedPtr<FBindingContext>& Context)>& InFunc)
{
	TArray<TSharedPtr<FBindingContext>> OutInputContexts;
	FInputBindingManager::Get().GetKnownInputContexts(OutInputContexts);
	for (const TSharedPtr<FBindingContext>& Context : OutInputContexts)
	{
		InFunc(Context);
	} 
}

void UEditorInputCommandsLibrary::ForeachCommandInContext(const FName& ContextName, const TFunctionRef<void(const TSharedPtr<FUICommandInfo>& Command)>& InFunc)
{
	FInputBindingManager& BindingManager = FInputBindingManager::Get();
	TArray<TSharedPtr<FUICommandInfo>> Commands;
	BindingManager.GetCommandInfosFromContext(ContextName, Commands);
	for (const TSharedPtr<FUICommandInfo>& Command : Commands)
	{
		InFunc(Command);
	}
}

TArray<FName> UEditorInputCommandsLibrary::GetCommandNames()
{
	TArray<FName> CommandNames;
	ForeachBindingContext([&CommandNames](const TSharedPtr<FBindingContext>& Context)
	{
		ForeachCommandInContext(Context->GetContextName(), [&CommandNames](const TSharedPtr<FUICommandInfo>& Command)
		{
			CommandNames.AddUnique(Command->GetCommandName());
		});
	});
	return CommandNames;
}

TArray<FName> UEditorInputCommandsLibrary::GetCommandListIdentifiers()
{
	const FEditorInputCommandsModule& Module = FModuleManager::GetModuleChecked<FEditorInputCommandsModule>(FEditorInputCommandsModule::GetModuleName());
	return Module.GetAvailableCommandListContexts();
}
