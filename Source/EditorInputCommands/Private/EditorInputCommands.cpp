// Copyright FifonszGames. All Rights Reserved.

#include "EditorInputCommands.h"

#include "BlueprintEditorModule.h"
#include "EditorInputCommand.h"
#include "EditorCustomizations/CommandIdentifierPinFactory.h"
#include "EditorCustomizations/EditorCommandStyle.h"
#include "EditorCustomizations/EditorInputCommandCustomization.h"
#include "EditorCustomizations/InputCommandIdentifierCustomization.h"

class FEditorCommandStyle;

namespace Utils
{
	FName GetPropertyEditorName() { return TEXT("PropertyEditor"); }
}

void FEditorInputCommandsModule::StartupModule()
{
	FEditorCommandStyle::Init();
	
	FInputBindingManager& Manager = FInputBindingManager::Get();
	Manager.OnRegisterCommandList.AddRaw(this, &FEditorInputCommandsModule::OnRegisterCommandList);
	Manager.OnUnregisterCommandList.AddRaw(this, &FEditorInputCommandsModule::OnUnregisterCommandList);
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(Utils::GetPropertyEditorName());
	PropertyEditorModule.RegisterCustomClassLayout(UEditorInputCommand::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FEditorInputCommandCustomization::MakeInstance));

	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FCommandIdentifier::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInputCommandIdentifierCustomization::MakeInstance));

	IdentifierPinFactory = MakeShared<FCommandIdentifierPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(IdentifierPinFactory);
}

void FEditorInputCommandsModule::ShutdownModule()
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

const TArray<TWeakPtr<FUICommandList>>* FEditorInputCommandsModule::GetCommandListsForContext(const FName& ContextName) const
{
	return CommandLists.Find(ContextName);
}

TArray<FName> FEditorInputCommandsModule::GetAvailableCommandListContexts() const
{
	TArray<FName> Keys;
	CommandLists.GenerateKeyArray(Keys);
	return Keys;
}

FName FEditorInputCommandsModule::GetModuleName()
{
	return TEXT("EditorInputCommands");
}

void FEditorInputCommandsModule::OnRegisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList)
{
	TArray<TWeakPtr<FUICommandList>>& List = CommandLists.FindOrAdd(ContextName);
	List.AddUnique(CommandList);
	List.RemoveAll([](const TWeakPtr<FUICommandList>& WeakList){ return !WeakList.IsValid(); });
}

void FEditorInputCommandsModule::OnUnregisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList)
{
	if (TArray<TWeakPtr<FUICommandList>>* ListArray = CommandLists.Find(ContextName))
	{
		ListArray->Remove(CommandList);
		ListArray->RemoveAll([](const TWeakPtr<FUICommandList>& WeakList){ return !WeakList.IsValid(); });
	}
}

void FEditorInputCommandsModule::TryAddUnregisteredLists()
{
	//ideally we don't have to manually add any list but unfortunately not every module registers its own
	
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	TArray<TWeakPtr<FUICommandList>>& Lists = CommandLists.FindOrAdd(TEXT("BlueprintEditor"));
	Lists.AddUnique(BlueprintEditorModule.GetsSharedBlueprintEditorCommands());
}

IMPLEMENT_MODULE(FEditorInputCommandsModule, EditorInputCommands)
