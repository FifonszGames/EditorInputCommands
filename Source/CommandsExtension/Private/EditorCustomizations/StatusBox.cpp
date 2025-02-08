// Copyright FifonszGames. All Rights Reserved.

#include "CommandsExtension/Public/EditorCustomizations/StatusBox.h"

#include "CommandsExtensionLibrary.h"
#include "EditorCustomizations/EditorCommandStyle.h"

#define AppIcon(Name) FAppStyle::Get().GetBrush(Name)

struct FStatusBoxData
{
	FStatusBoxData(const FSlateBrush* InImageBrush, const FText& InStatusText, const FSlateColor& InBorderColor) 
		:  ImageBrush(InImageBrush), StatusText(InStatusText), StatusColor(InBorderColor) {}

	const FSlateBrush* ImageBrush;
	FText StatusText;
	FSlateColor StatusColor;
};

static const TMap<EStatusBoxState, FStatusBoxData> DefaultStatusData =
{
	{EStatusBoxState::Error,
		{
			FStatusBoxData(AppIcon("Icons.ErrorWithColor"),
			TextFromString("Error"),
			FStyleColors::Error)
		}},
	{EStatusBoxState::Warning,
		{
			FStatusBoxData(AppIcon("Icons.WarningWithColor"),
				TextFromString("Warning"),
				FStyleColors::Warning)}},
	{EStatusBoxState::Success,
		{
			FStatusBoxData(AppIcon("Icons.SuccessWithColor"),
				TextFromString("Success"),
				FStyleColors::Success)}},	
};

void SStatusBox::Construct(const FArguments& InArgs)
{
	BoxStatus = InArgs._BoxStatus;
	
	SBorder::Construct(SBorder::FArguments()
		.Padding(InArgs._Padding)
		.ForegroundColor(FAppStyle::Get().GetSlateColor("Colors.Red"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f))
			[
				SAssignNew(StatusImage, SImage)
				.DesiredSizeOverride(InArgs._IconSize)
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SAssignNew(StatusText, STextBlock)
				.ColorAndOpacity(FAppStyle::Get().GetSlateColor("Colors.White"))
				.AutoWrapText(InArgs._AutoWrapText)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				InArgs._Content.Widget
			]
		]);
		
	OnStatusChanged(BoxStatus.Get());
}

void SStatusBox::SetStatus(EStatusBoxState NewStatus)
{
	if (BoxStatus.IsSet() && BoxStatus.Get() == NewStatus)
	{
		return;
	}
	BoxStatus = NewStatus;
	OnStatusChanged(NewStatus);
}

void SStatusBox::OnStatusChanged(EStatusBoxState NewStatus)
{
	if (const FSlateBrush* Brush = GetBorderBrush(NewStatus))
	{
		BorderBrush = *Brush;
	}
	BorderBrush.OutlineSettings.Color = StatusToColor(NewStatus);
	
	SetBorderImage(&BorderBrush);
	StatusImage->SetImage(GetImageBrush(NewStatus));
	StatusText->SetText(GetStatusText(NewStatus));
}

const FSlateBrush* SStatusBox::GetBorderBrush(EStatusBoxState ForStatus) const
{
	return FEditorCommandStyle::Get().GetBrush(FEditorCommandStyle::GetOutlineStyleName());
}

const FSlateBrush* SStatusBox::GetImageBrush(EStatusBoxState ForStatus) const
{
	return DefaultStatusData[ForStatus].ImageBrush;
}

FText SStatusBox::GetStatusText(EStatusBoxState ForStatus) const
{
	return DefaultStatusData[ForStatus].StatusText;
}

FSlateColor SStatusBox::StatusToColor(EStatusBoxState Status) const
{
	return DefaultStatusData[Status].StatusColor;
}
