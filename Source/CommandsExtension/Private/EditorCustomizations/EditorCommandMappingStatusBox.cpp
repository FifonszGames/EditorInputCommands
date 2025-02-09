// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandMappingStatusBox.h"

#include "CommandsExtensionLibrary.h"
#include "EditorInputCommand.h"

void SEditorCommandMappingStatusBox::Construct(const FArguments& InArgs)
{
	TargetCommand = InArgs._TargetCommand;
	
	SStatusBox::Construct(SStatusBox::FArguments()
		.Padding(InArgs._Padding)
		.IconSize(InArgs._IconSize)
		.AutoWrapText(InArgs._AutoWrapText)
		[
			InArgs._Content.Widget
		]);
	
	RefreshState(true);
}

void SEditorCommandMappingStatusBox::RefreshState(const bool bInForceIfSame)
{
	if (const UEditorInputCommand* Command = TargetCommand.Get())
	{
		if (!Command->CurrentIdentifier.IsRegistered())
		{
			SetStatus(EStatusBoxState::Warning, bInForceIfSame);
		}
		else if (Command->MappedLists.IsEmpty())
		{
			SetStatus(EStatusBoxState::Error, bInForceIfSame);
		}
		else
		{
			SetStatus(EStatusBoxState::Success, bInForceIfSame);
		}
	}
	else
	{
		SetStatus(EStatusBoxState::Error, bInForceIfSame);
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
