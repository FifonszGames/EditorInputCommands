// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "StatusBox.h"

class UEditorInputCommand;

class SEditorCommandRegistrationStatusBox : public SStatusBox
{
public:
	SLATE_STATUS_BOX_ARGS(SEditorCommandRegistrationStatusBox)
	SLATE_ARGUMENT(TWeakObjectPtr<const UEditorInputCommand>, TargetCommand)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void RefreshState();

private:
	TWeakObjectPtr<const UEditorInputCommand> TargetCommand;
	FSlateBrush BorderBrush;
};
