// Copyright FifonszGames. All Rights Reserved.

#include "CommandsExtension.h"

#include "EdGraphUtilities.h"
#include "EditorInputCommand.h"
#include "EditorCustomizations/CommandIdentifierPinFactory.h"
#include "EditorCustomizations/EditorCommandStyle.h"
#include "EditorCustomizations/InputCommandCustomization.h"
#include "EditorCustomizations/InputCommandIdentifierCustomization.h"

class FEditorCommandStyle;

namespace Utils
{
	FName GetPropertyEditorName() { return TEXT("PropertyEditor"); }
}

void FCommandsExtensionModule::StartupModule()
{
	FEditorCommandStyle::Init();
	
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.AddRaw(this, &FCommandsExtensionModule::OnRegisterCommandList);
	Manager.OnUnregisterCommandList.AddRaw(this, &FCommandsExtensionModule::OnUnregisterCommandList);
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(Utils::GetPropertyEditorName());
	PropertyEditorModule.RegisterCustomClassLayout(UEditorInputCommand::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FInputCommandCustomization::MakeInstance));

	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FCommandIdentifier::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInputCommandIdentifierCustomization::MakeInstance));

	IdentifierPinFactory = MakeShared<FCommandIdentifierPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(IdentifierPinFactory);
}

void FCommandsExtensionModule::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(IdentifierPinFactory);
	
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.RemoveAll(this);
	Manager.OnUnregisterCommandList.RemoveAll(this);
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(Utils::GetPropertyEditorName());
	PropertyEditorModule.UnregisterCustomClassLayout(UEditorInputCommand::StaticClass()->GetFName());
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FCommandIdentifier::StaticStruct()->GetFName());
	
	FEditorCommandStyle::Shutdown();
}

TWeakPtr<FUICommandList> FCommandsExtensionModule::GetCommandListForContext(const FName& ContextName) const
{
	const TWeakPtr<FUICommandList>* WeakPtr = CommandLists.Find(ContextName);
	return WeakPtr ? *WeakPtr : TWeakPtr<FUICommandList>();
}

TArray<FName> FCommandsExtensionModule::GetAvailableContexts() const
{
	TArray<FName> Keys;
	CommandLists.GenerateKeyArray(Keys);
	return Keys;
}

FName FCommandsExtensionModule::GetModuleName()
{
	return TEXT("CommandsExtension");
}

void FCommandsExtensionModule::OnRegisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList)
{
	if (!CommandLists.Contains(ContextName))
	{
		CommandLists.Add(ContextName, CommandList);
	}
	else
	{
		CommandLists[ContextName] = CommandList;
	}
}

void FCommandsExtensionModule::OnUnregisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList)
{
	if (CommandLists.Contains(ContextName))
	{
		CommandLists.Remove(ContextName);
	}
}

IMPLEMENT_MODULE(FCommandsExtensionModule, CommandsExtension)
