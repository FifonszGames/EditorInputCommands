// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionTypes.h"

#include "CommandsExtension.h"
#include "CommandsExtensionLibrary.h"
#include "EditorInputCommand.h"
#include "EditorCustomizations/EditorCommandStyle.h"

TSharedPtr<FBindingContext> FBindingContextProvider::AsContext() const
{
	return FInputBindingManager::Get().GetContextByName(GetBindingContextName());
}

FNewContextBinding::FNewContextBinding(const FName BindingContextName, const FText& InContextDescription) : BindingContextName(BindingContextName), ContextDescription(InContextDescription)
{
	CreateContext();
}

TSharedPtr<FBindingContext> FNewContextBinding::AsContext() const
{
	CreateContext();
	return Context;
}

void FNewContextBinding::CreateContext() const
{
	if (Context.IsValid())
	{
		if (Context->GetContextName().IsEqual(BindingContextName))
		{
			return;
		}
		FInputBindingManager::Get().RemoveContextByName(Context->GetContextName());
		Context.Reset();
	}
	if (!ContextDescription.IsEmpty() && !BindingContextName.IsNone() && !UCommandsExtensionLibrary::GetBindingContextNames().Contains(BindingContextName))
	{
		Context = MakeShareable(new FBindingContext(BindingContextName,
		ContextDescription,
			NAME_None,
			FEditorCommandStyle::Get().GetStyleSetName()));
	}
}

FInputCommandRegisterData::FInputCommandRegisterData() : ContextProvider(FInstancedStruct(Cast<UScriptStruct>(FExistingContextBinding::StaticStruct())))
{
}

bool FInputCommandRegisterData::IsValid() const
{
	if (const FBindingContextProvider* Provider = ContextProvider.GetPtr<FBindingContextProvider>())
	{
		return Provider->IsValid() && !Identifier.IsNone() && !Label.IsEmpty() && !Description.IsEmpty();	
	}
	return false;
}

TSharedPtr<FBindingContext> FInputCommandRegisterData::GetContext() const
{
	const FBindingContextProvider* Provider = ContextProvider.GetPtr<FBindingContextProvider>();
	return Provider ? Provider->AsContext() : nullptr;
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

FCommandIdentifier::FCommandIdentifier(const FInputCommandRegisterData& Data) : Identifier(Data.Identifier)
{
	if (const FBindingContextProvider* Provider = Data.ContextProvider.GetPtr<FBindingContextProvider>())
	{
		BindingContext = Provider->GetBindingContextName();
	}
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
