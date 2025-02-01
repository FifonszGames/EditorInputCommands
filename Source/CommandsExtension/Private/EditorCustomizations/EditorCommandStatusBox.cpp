// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandStatusBox.h"

#include "CommandsExtensionLibrary.h"

void SEditorCommandStatusBox::Construct(const FArguments& InArgs)
{
	CommandHandle = InArgs._CommandHandle;
	UObject* Value = nullptr;
	FPropertyAccess::Result Result = CommandHandle->GetValue(Value);
	if (Result == FPropertyAccess::Result::Success)
	{
		CommandHandle->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateSP(this, &SEditorCommandStatusBox::OnChildChanged));
	}
	SStatusBox::Construct(SStatusBox::FArguments()
		.BoxStatus(ERegistrationResult::Success)
		.Padding(InArgs._Padding)
		.IconSize(InArgs._IconSize)
		.AutoWrapText(InArgs._AutoWrapText));
}

const FSlateBrush* SEditorCommandStatusBox::GetImageBrush(ERegistrationResult ForStatus) const
{
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

void SEditorCommandStatusBox::OnChildChanged()
{
	int32 a = 32;
}
