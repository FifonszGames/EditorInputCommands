// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "StatusBox.h"

class UEditorInputCommand;
enum class ERegistrationResult : uint8;

class SEditorCommandStatusBox : public SStatusBox<ERegistrationResult>
{
public:
	SLATE_BEGIN_ARGS(SEditorCommandStatusBox)
		: _Padding(16.0f)
		, _IconSize(24,24)
		, _AutoWrapText(true)
		{}
		
	SLATE_ARGUMENT(FMargin, Padding)
	SLATE_ARGUMENT(FVector2D, IconSize)
	SLATE_ARGUMENT(bool, AutoWrapText)
	SLATE_ARGUMENT(TWeakObjectPtr<UEditorInputCommand>, TargetCommand)	
		
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	void RefreshState();

protected:
	virtual const FSlateBrush* GetImageBrush(ERegistrationResult ForStatus) const override;
	virtual const FSlateBrush* GetBorderBrush(ERegistrationResult ForStatus) const override;
	virtual FText GetStatusText(ERegistrationResult NewStatus) const override;

private:
	TWeakObjectPtr<UEditorInputCommand> TargetCommand;
};
