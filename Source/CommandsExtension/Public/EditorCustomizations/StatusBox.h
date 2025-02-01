// Copyright FifonszGames. All Rights Reserved.

#pragma once

template<typename StatusEnum>
class SStatusBox : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SStatusBox)
		: _Padding(16.0f)
		, _IconSize(24,24)
		, _AutoWrapText(true)
		, _Content()
	{}
	SLATE_ATTRIBUTE(StatusEnum, BoxStatus)
	SLATE_ARGUMENT(FMargin, Padding)
	SLATE_ARGUMENT(FVector2D, IconSize)
	SLATE_ARGUMENT(bool, AutoWrapText)

	SLATE_DEFAULT_SLOT(FArguments, Content)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		BoxStatus = InArgs._BoxStatus;
		
		SBorder::Construct(SBorder::FArguments()
			.Padding(InArgs._Padding)
			.ForegroundColor(FAppStyle::Get().GetSlateColor("Colors.White"))
			.BorderImage_Lambda([this](){ return GetBorderBrush(BoxStatus.Get()); })
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
					.Image_Lambda([this]() { return GetImageBrush(BoxStatus.Get()); })
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SAssignNew(StatusText, STextBlock)
					.Text_Lambda([this]() { return GetStatusText(BoxStatus.Get()); })
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
	
	void SetStatus(StatusEnum NewStatus)
	{
		if (BoxStatus.IsSet() && BoxStatus.Get() == NewStatus)
		{
			return;
		}
		BoxStatus = NewStatus;
		OnStatusChanged(NewStatus);
	}

protected:
	virtual void OnStatusChanged(StatusEnum NewStatus)
	{
		// StatusImage->SetImage(GetImageBrush(NewStatus));
		StatusText->SetText(GetStatusText(NewStatus));
	}

	virtual const FSlateBrush* GetBorderBrush(StatusEnum ForStatus) const { return FAppStyle::Get().GetBrush("RoundedError"); }
	virtual const FSlateBrush* GetImageBrush(StatusEnum ForStatus) const { return FAppStyle::Get().GetBrush("Icons.WarningWithColor"); }
	virtual FText GetStatusText(StatusEnum ForStatus) const = 0;

private:
	TAttribute<StatusEnum> BoxStatus;
	
	TSharedPtr<STextBlock> StatusText;
	TSharedPtr<SImage> StatusImage;
};