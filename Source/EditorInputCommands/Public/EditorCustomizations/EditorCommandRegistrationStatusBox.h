// Copyright FifonszGames 2025 All Rights Reserved.

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
	void RefreshState(const bool bInForceIfSame = false);

protected:
	virtual FText GetStatusText(EStatusBoxState ForStatus) const override;

private:
	TWeakObjectPtr<const UEditorInputCommand> TargetCommand;
	FSlateBrush BorderBrush;
};
