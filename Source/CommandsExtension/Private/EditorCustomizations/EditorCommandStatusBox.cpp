// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandStatusBox.h"

#include "CommandsExtensionLibrary.h"

void SEditorCommandStatusBox::Construct(const FArguments& InArgs)
{
	TargetCommand = InArgs._TargetCommand;

	SStatusBox::Construct(SStatusBox::FArguments()
		.BoxStatus(ERegistrationResult::Success)
		.Padding(InArgs._Padding)
		.IconSize(InArgs._IconSize)
		.AutoWrapText(InArgs._AutoWrapText));
	
	RefreshState();
}

void SEditorCommandStatusBox::RefreshState()
{
	if (UEditorInputCommand* Command = TargetCommand.Get())
	{
		
	}
	else
	{
		SetStatus(ERegistrationResult::InvalidRegistrationData);
	}
}

const FSlateBrush* SEditorCommandStatusBox::GetImageBrush(ERegistrationResult ForStatus) const
{
	const FSlateBrush* BaseBrush = SStatusBox::GetImageBrush(ForStatus);
	return SStatusBox::GetImageBrush(ForStatus);
}

const FSlateBrush* SEditorCommandStatusBox::GetBorderBrush(ERegistrationResult NewStatus) const
{
	return SStatusBox::GetBorderBrush(NewStatus);
}

FText SEditorCommandStatusBox::GetStatusText(ERegistrationResult ForStatus) const
{
	return FText::FromString(TEXT("YEEEET"));
}
