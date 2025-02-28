// Copyright FifonszGames. All Rights Reserved.


#include "EditorInputCommand.h"

#include "CommandsExtensionLibrary.h"
#include "EditorUtilityObject.h"

void UEditorInputCommand::RegisterCommand()
{
	UnregisterCommand();
	
	ERegistrationResult Result;
	UCommandsExtensionLibrary::RegisterInputCommand(RegistrationData, Result);
	if (Result == ERegistrationResult::Success)
	{
		CurrentIdentifier = RegistrationData.GetIdentifier();
		MapToTargetList();
	}
}

void UEditorInputCommand::UnregisterCommand()
{
	if (CurrentIdentifier.IsRegistered())
	{
		for (const FCommandListIdentifier& Identifier : MappedLists)
		{
			UCommandsExtensionLibrary::UnmapAction(Identifier, CurrentIdentifier, ExecuteDelegate);	
		}
		UCommandsExtensionLibrary::UnregisterInputCommand(CurrentIdentifier);
	}
	CurrentIdentifier = FCommandIdentifier();
	MappedLists.Empty();
}

void UEditorInputCommand::MapToTargetList()
{
	if (TargetList.IsValid() && CurrentIdentifier.IsRegistered())
	{
		UnmapFromTargetList();

		if (!ExecuteDelegate.IsBound())
		{
			ExecuteDelegate.BindDynamic(this, &UEditorInputCommand::ExecuteCommand);
		}
		if (UCommandsExtensionLibrary::MapAction(TargetList, CurrentIdentifier, ExecuteDelegate, RepeatMode))
		{
			MappedLists.Add(TargetList);
		}
	}
}

void UEditorInputCommand::UnmapFromTargetList()
{
	if (TargetList.IsValid() && CurrentIdentifier.IsRegistered() && MappedLists.Contains(TargetList))
	{
		UCommandsExtensionLibrary::UnmapAction(TargetList, CurrentIdentifier, ExecuteDelegate);
		MappedLists.Remove(TargetList);
	}
}

void UEditorInputCommand::BeginDestroy()
{
	UnregisterCommand();
	Super::BeginDestroy();
}

void UEditorInputCommand::ExecuteCommand()
{
	for (const auto& EditorUtilityObject : RunnableObjects)
	{
		if (const UClass* UtilityObjectClass = EditorUtilityObject.LoadSynchronous())
		{
			if (UEditorUtilityObject* Object = UtilityObjectClass->GetDefaultObject<UEditorUtilityObject>())
			{
				Object->Run();
			}
		}
	} 
	OnInputCommandExecuted.Broadcast();
}
