﻿// Copyright FifonszGames. All Rights Reserved.

#include "CommandsExtensionLibrary.h"

#include "CommandExtensionSubsystem.h"
#include "CommandsExtension.h"
#include "EditorInputCommand.h"

namespace CMDUtils
{
	EUIActionRepeatMode Convert(ECommandRepeatMode Mode)
	{
		return Mode == ECommandRepeatMode::Enabled ? EUIActionRepeatMode::RepeatEnabled : EUIActionRepeatMode::RepeatDisabled;
	}
}

void UCommandsExtensionLibrary::RegisterInputCommand(const FInputCommandRegisterData& InData, ERegistrationResult& OutResult)
{
	if (!InData.IsValid())
	{
		OutResult = ERegistrationResult::InvalidRegistrationData;
		return;
	}
	
	TSharedPtr<FBindingContext> BindingContext = InData.GetContext();
	if (!BindingContext.IsValid())
	{
		OutResult = ERegistrationResult::InvalidContext;
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
		BindingContext.ToSharedRef(), 
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

bool UCommandsExtensionLibrary::UnregisterInputCommand(const FCommandIdentifier& CommandIdentifier)
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

bool UCommandsExtensionLibrary::MapAction(const FCommandListIdentifier& TargetList, const FCommandIdentifier& CommandIdentifier, FOnExecute OnExecute, ECommandRepeatMode RepeatMode)
{
	TSharedPtr<FUICommandInfo> CommandInfo = CommandIdentifier.AsInfo();
	if (CommandInfo.IsValid())
	{
		if (UCommandExtensionSubsystem* Subsystem = GEditor->GetEditorSubsystem<UCommandExtensionSubsystem>())
		{
			const EUIActionRepeatMode Mode = CMDUtils::Convert(RepeatMode);
			return TargetList.ForeachCommandList([&CommandInfo, Subsystem, &OnExecute, Mode](const TSharedRef<FUICommandList>& List)
			{
				return Subsystem->MapAction(List, CommandInfo.ToSharedRef(), OnExecute, Mode);
			});
		}
	}
	return false;
}

bool UCommandsExtensionLibrary::MapActionToCommand(UEditorInputCommand* CommandAsset, FOnExecute OnExecute)
{
	if (CommandAsset && OnExecute.IsBound())
	{
		CommandAsset->OnInputCommandExecuted.AddUnique(OnExecute);
		return true;
	}
	return false;
}

bool UCommandsExtensionLibrary::UnmapAction(const FCommandListIdentifier& TargetList, const FCommandIdentifier& CommandIdentifier)
{
	TSharedPtr<FUICommandInfo> CommandInfo = CommandIdentifier.AsInfo();
	if (CommandInfo.IsValid())
	{
		if (UCommandExtensionSubsystem* Subsystem = GEditor->GetEditorSubsystem<UCommandExtensionSubsystem>())
		{
			TargetList.ForeachCommandList([&CommandInfo, Subsystem](const TSharedRef<FUICommandList>& List)
			{
				return Subsystem->UnMapAction(List, CommandInfo.ToSharedRef());
			});
		}
	}
	return false;
}

bool UCommandsExtensionLibrary::UnmapActionFromCommand(UEditorInputCommand* CommandAsset, FOnExecute OnExecute)
{
	if (CommandAsset && CommandAsset->OnInputCommandExecuted.Contains(OnExecute))
	{
		CommandAsset->OnInputCommandExecuted.Remove(OnExecute);
		return true;
	}
	return false;
}

bool UCommandsExtensionLibrary::GetUserDefinedCommandInputChord(const FCommandIdentifier& CommandIdentifier, const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord)
{
	return CommandIdentifier.GetUserDefinedChord(InChordIndex, OutChord);
}

bool UCommandsExtensionLibrary::IsCommandRegistered(const FCommandIdentifier& CommandIdentifier)
{
	return CommandIdentifier.IsRegistered();
}

FCommandIdentifier UCommandsExtensionLibrary::MakeCommandIdentifier(const FName BindingContext, const FName Identifier)
{
	return FCommandIdentifier(BindingContext, Identifier);
}

FExistingContextBinding UCommandsExtensionLibrary::MakeExistingContextBinding(const FName BindingContextName)
{
	return FExistingContextBinding(BindingContextName);
}

TArray<FName> UCommandsExtensionLibrary::GetBindingContextNames()
{
	TArray<FName> Names;
	ForeachBindingContext([&Names](const TSharedPtr<FBindingContext>& Context)
	{
		Names.AddUnique(Context->GetContextName());
	});
	return Names;
}

void UCommandsExtensionLibrary::ForeachBindingContext(const TFunctionRef<void(const TSharedPtr<FBindingContext>& Context)>& InFunc)
{
	TArray<TSharedPtr<FBindingContext>> OutInputContexts;
	FInputBindingManager::Get().GetKnownInputContexts(OutInputContexts);
	for (const TSharedPtr<FBindingContext>& Context : OutInputContexts)
	{
		InFunc(Context);
	} 
}

void UCommandsExtensionLibrary::ForeachCommandInContext(const FName& ContextName, const TFunctionRef<void(const TSharedPtr<FUICommandInfo>& Command)>& InFunc)
{
	FInputBindingManager& BindingManager = FInputBindingManager::Get();
	TArray<TSharedPtr<FUICommandInfo>> Commands;
	BindingManager.GetCommandInfosFromContext(ContextName, Commands);
	for (const TSharedPtr<FUICommandInfo>& Command : Commands)
	{
		InFunc(Command);
	}
}

TArray<FName> UCommandsExtensionLibrary::GetCommandNames()
{
	TArray<FName> CommandNames;
	FInputBindingManager& BindingManager = FInputBindingManager::Get();
	
	TArray<TSharedPtr<FBindingContext>> OutInputContexts;
	BindingManager.GetKnownInputContexts(OutInputContexts);
	
	for (const TSharedPtr<FBindingContext>& Context : OutInputContexts)
	{
		ForeachCommandInContext(Context->GetContextName(), [&CommandNames](const TSharedPtr<FUICommandInfo>& Command)
		{
			CommandNames.AddUnique(Command->GetCommandName());
		});
	}
	return CommandNames;
}

TArray<FName> UCommandsExtensionLibrary::GetCommandListIdentifiers()
{
	const FCommandsExtensionModule& Module = FModuleManager::GetModuleChecked<FCommandsExtensionModule>(FCommandsExtensionModule::GetModuleName());
	return Module.GetAvailableContexts();
}
