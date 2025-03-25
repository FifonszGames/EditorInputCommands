// Copyright FifonszGames 2025 All Rights Reserved.


#include "EditorInputCommandTypes.h"

#include "EditorInputCommands.h"
#include "EditorInputCommand.h"
#include "Algo/AnyOf.h"
#include "EditorCustomizations/EditorCommandStyle.h"

TSharedPtr<FBindingContext> FBindingContextProvider::AsContext() const
{
	return FInputBindingManager::Get().GetContextByName(GetBindingContextName());
}

FInputCommandRegisterData::FInputCommandRegisterData(const TSharedRef<FBindingContext>& Context, const TSharedRef<FUICommandInfo>& CommandInfo)
	: ContextProvider(FExistingContextBinding(Context->GetContextName())), Identifier(CommandInfo->GetCommandName()), Label(CommandInfo->GetLabel()),
	Description(CommandInfo->GetDescription()), DefaultChord(CommandInfo->GetDefaultChord(EMultipleKeyBindingIndex::Primary)),
	AlternateDefaultChord(CommandInfo->GetDefaultChord(EMultipleKeyBindingIndex::Primary))
{
}

bool FInputCommandRegisterData::IsValid() const
{
	return ContextProvider.IsValid() && !Identifier.IsNone() && !Label.IsEmpty() && !Description.IsEmpty();	
}

TSharedPtr<FBindingContext> FInputCommandRegisterData::GetContext() const
{
	return ContextProvider.AsContext();
}

FCommandIdentifier FInputCommandRegisterData::GetIdentifier() const
{
	return FCommandIdentifier(*this);
}

FSlateIcon FInputCommandRegisterData::GetIcon()
{
	return FSlateIcon(FEditorCommandStyle::Get().GetStyleSetName(),FEditorCommandStyle::GetClassIconStyleSetName<UEditorInputCommand>());
}

FCommandIdentifier::FCommandIdentifier(const FInputCommandRegisterData& Data) : BindingContext(Data.ContextProvider.GetBindingContextName()), Identifier(Data.Identifier) {}

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

bool FCommandListIdentifier::ForeachCommandList(const TFunctionRef<bool(const TSharedRef<FUICommandList>& List)>& InFunc) const
{
	bool bAtleastOneSucceded = false;
	if (const TArray<TWeakPtr<FUICommandList>>* ListsArray = GetRelatedCommandLists())
	{
		for (const TWeakPtr<FUICommandList>& WeakList : *ListsArray)
		{
			if (WeakList.IsValid())
			{
				bAtleastOneSucceded |= InFunc(WeakList.Pin().ToSharedRef());
			}
		}
	}
	return bAtleastOneSucceded;
}

const TArray<TWeakPtr<FUICommandList>>* FCommandListIdentifier::GetRelatedCommandLists() const
{
	const FEditorInputCommandsModule& Module = FModuleManager::GetModuleChecked<FEditorInputCommandsModule>(FEditorInputCommandsModule::GetModuleName());
	return Module.GetCommandListsForContext(Identifier);
}

bool FCommandListIdentifier::IsValid() const
{
	return !Identifier.IsNone() && HasValidCommandList();
}

bool FCommandListIdentifier::HasValidCommandList() const
{
	const TArray<TWeakPtr<FUICommandList>>* ListsArray = GetRelatedCommandLists();
	return ListsArray && Algo::AnyOf(*ListsArray, [](const TWeakPtr<FUICommandList>& WeakList) { return WeakList.IsValid(); });
}
