// Copyright FifonszGames. All Rights Reserved.


#include "EditorInputCommand.h"

#include "CommandsExtensionLibrary.h"

void UEditorInputCommand::RegisterCommand()
{
	UnregisterCommand();
	
	ERegistrationResult Result;
	UCommandsExtensionLibrary::RegisterInputCommand(RegistrationData, Result);
	switch (Result)
	{
		case ERegistrationResult::Success:
			{
				CurrentIdentifier = RegistrationData.GetIdentifier();
				if (!ExecuteDelegate.IsBound())
				{
					ExecuteDelegate.BindDynamic(this, &UEditorInputCommand::ExecuteCommand);
				}
				MapToTargetList();
			}
			break;
		default:
			//TODO:: add editor popup for fail
			break;
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

void UEditorInputCommand::ExecuteCommand()
{
	Run();
	OnInputCommandExecuted.Broadcast();
}
