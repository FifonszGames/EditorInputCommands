// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandRegistrationStatusBox.h"

#include "EditorInputCommand.h"

void SEditorCommandRegistrationStatusBox::Construct(const FArguments& InArgs)
{
	TargetCommand = InArgs._TargetCommand;
	
	SStatusBox::Construct(SStatusBox::FArguments()
		.BoxStatus(EStatusBoxState::Error)
		.Padding(InArgs._Padding)
		.IconSize(InArgs._IconSize)
		.AutoWrapText(InArgs._AutoWrapText)
		[
			InArgs._Content.Widget
		]);
	
	RefreshState();
}

void SEditorCommandRegistrationStatusBox::RefreshState()
{
	if (const UEditorInputCommand* Command = TargetCommand.Get())
	{
		if (Command->RegistrationData.GetIdentifier() == Command->CurrentIdentifier)
		{
			// SetStatus(EEditorCommandRegistrationStatus::Registered);
		}
	}
	else
	{
		// SetStatus(ERegistrationResult::InvalidRegistrationData);
	}
}