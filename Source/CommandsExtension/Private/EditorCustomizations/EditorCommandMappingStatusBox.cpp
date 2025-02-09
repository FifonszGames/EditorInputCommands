// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandMappingStatusBox.h"

#include "CommandsExtensionLibrary.h"
#include "EditorInputCommand.h"

void SEditorCommandMappingStatusBox::Construct(const FArguments& InArgs)
{
	TargetCommand = InArgs._TargetCommand;
	
	SStatusBox::Construct(SStatusBox::FArguments()
		.BoxStatus(InArgs._BoxStatus)
		.Padding(InArgs._Padding)
		.IconSize(InArgs._IconSize)
		.AutoWrapText(InArgs._AutoWrapText)
		[
			InArgs._Content.Widget
		]);
	
	RefreshState();
}

void SEditorCommandMappingStatusBox::RefreshState()
{
	if (const UEditorInputCommand* Command = TargetCommand.Get())
	{
		if (!Command->CurrentIdentifier.IsRegistered())
		{
			SetStatus(EStatusBoxState::Warning);
		}
		else if (Command->MappedLists.IsEmpty())
		{
			SetStatus(EStatusBoxState::Error);
		}
		else
		{
			SetStatus(EStatusBoxState::Success);
		}
	}
	else
	{
		SetStatus(EStatusBoxState::Error);
	}
}

FText SEditorCommandMappingStatusBox::GetStatusText(EStatusBoxState ForStatus) const
{
	switch (ForStatus)
	{
		case EStatusBoxState::Success:
			return STRING_TO_TEXT("Command is mapped");
		case EStatusBoxState::Warning:
			return STRING_TO_TEXT("Command is not registered");
		default:
			return STRING_TO_TEXT("Command is not mapped to any list");
	}
}
