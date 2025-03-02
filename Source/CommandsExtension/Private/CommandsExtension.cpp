// Copyright FifonszGames. All Rights Reserved.

#include "CommandsExtension.h"

#include "BlueprintEditorModule.h"
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

const TArray<TWeakPtr<FUICommandList>>* FCommandsExtensionModule::GetCommandListsForContext(const FName& ContextName) const
{
	return CommandLists.Find(ContextName);
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
	TArray<TWeakPtr<FUICommandList>>& List = CommandLists.FindOrAdd(ContextName);
	List.AddUnique(CommandList);
	List.RemoveAll([](const TWeakPtr<FUICommandList>& WeakList){ return !WeakList.IsValid(); });
}

void FCommandsExtensionModule::OnUnregisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList)
{
	if (TArray<TWeakPtr<FUICommandList>>* ListArray = CommandLists.Find(ContextName))
	{
		ListArray->Remove(CommandList);
		ListArray->RemoveAll([](const TWeakPtr<FUICommandList>& WeakList){ return !WeakList.IsValid(); });
	}
}

void FCommandsExtensionModule::TryAddUnregisteredLists()
{
	//ideally we don't have to manually any list but unfortunately not every module registers its own command list
	
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	TArray<TWeakPtr<FUICommandList>>& Lists = CommandLists.FindOrAdd(TEXT("BlueprintEditor"));
	Lists.Add(BlueprintEditorModule.GetsSharedBlueprintEditorCommands());
}

IMPLEMENT_MODULE(FCommandsExtensionModule, CommandsExtension)
