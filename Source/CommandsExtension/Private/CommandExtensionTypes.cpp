// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionTypes.h"

#include "CommandsExtension.h"
#include "CommandsExtensionLibrary.h"
#include "EditorInputCommand.h"
#include "EditorCustomizations/EditorCommandStyle.h"

void FNewContextBinding::Refresh()
{
	if (Context.IsValid())
	{
		if (Context->GetContextName().IsEqual(BindingContext))
		{
			return;
		}
		FInputBindingManager::Get().RemoveContextByName(Context->GetContextName());
		Context.Reset();
	}
	if (UCommandsExtensionLibrary::GetBindingContextNames().Contains(BindingContext))
	{
		//context with this name already exists
	}
	else
	{
		Context = MakeShareable(new FBindingContext(BindingContext,
			FText::FromString(TEXT("Editor Input Commands")),
			NAME_None,
			NAME_None));
	}
}

bool FInputCommandRegisterData::IsValid() const
{
	const FCommandIdentifier CommandIdentifier = GetIdentifier();
	return CommandIdentifier.CanEverPointToValidCommand() && !Label.IsEmpty() && !Description.IsEmpty();
}

FCommandIdentifier FInputCommandRegisterData::GetIdentifier() const
{
	return FCommandIdentifier(*this);
}

FSlateIcon FInputCommandRegisterData::GetIcon()
{
	return FSlateIcon(FEditorCommandStyle::Get().GetStyleSetName(),
		FEditorCommandStyle::GetClassIconStyleSetName<UEditorInputCommand>());
}

TSharedPtr<FUICommandInfo> FCommandIdentifier::AsInfo() const
{
	return FInputBindingManager::Get().FindCommandInContext(BindingContext, Identifier);
}

TSharedPtr<FBindingContext> FCommandIdentifier::AsContext() const
{
	return FInputBindingManager::Get().GetContextByName(BindingContext);
}

bool FCommandIdentifier::CanEverPointToValidCommand() const
{
	return !Identifier.IsNone() && !BindingContext.IsNone();
}

bool FCommandIdentifier::IsRegistered() const
{
	return CanEverPointToValidCommand() && AsInfo().IsValid();
}

bool FCommandIdentifier::GetUserDefinedChord(const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord) const
{
	return FInputBindingManager::Get().GetUserDefinedChord(BindingContext, Identifier, InChordIndex, OutChord);
}

TWeakPtr<FUICommandList> FCommandListIdentifier::AsCommandList() const
{
	const FCommandsExtensionModule& Module = FModuleManager::GetModuleChecked<FCommandsExtensionModule>(FCommandsExtensionModule::GetModuleName());
	return Module.GetCommandListForContext(Identifier);
}

bool FCommandListIdentifier::IsValid() const
{
	return !Identifier.IsNone() && AsCommandList().IsValid();
}
