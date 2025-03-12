// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandRegistrationStatusBox.h"

#include "EditorInputCommandsLibrary.h"
#include "EditorInputCommand.h"

void SEditorCommandRegistrationStatusBox::Construct(const FArguments& InArgs)
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

void SEditorCommandRegistrationStatusBox::RefreshState(const bool bInForceIfSame)
{
	if (const UEditorInputCommand* Command = TargetCommand.Get())
	{
		if (!Command->CurrentIdentifier.IsRegistered())
		{
			SetStatus(EStatusBoxState::Error, bInForceIfSame);
		}
		else if (Command->RegistrationData.GetIdentifier() == Command->CurrentIdentifier)
		{
			SetStatus(EStatusBoxState::Success, bInForceIfSame);
		}
		else
		{
			SetStatus(EStatusBoxState::Warning, bInForceIfSame);
		}
	}
	else
	{
		SetStatus(EStatusBoxState::Error, bInForceIfSame);
	}
}

FText SEditorCommandRegistrationStatusBox::GetStatusText(EStatusBoxState ForStatus) const
{
	switch (ForStatus)
	{
		case EStatusBoxState::Success:
			return STRING_TO_TEXT("Command is registered");
		case EStatusBoxState::Warning:
			return FText::FromString(FString::Printf(TEXT("%s's identification info differs from %s"),
				GET_MEMBER_NAME_STRING_CHECKED(UEditorInputCommand, RegistrationData),
				GET_MEMBER_NAME_STRING_CHECKED(UEditorInputCommand, CurrentIdentifier)));
		default:
			return STRING_TO_TEXT("Command is not registered");
	}

}
