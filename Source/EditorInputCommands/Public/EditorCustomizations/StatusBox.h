// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once

#include "Widgets/Layout/SBorder.h"
#include "Templates/SharedPointer.h"

#define SLATE_STATUS_BOX_ARGS(Type) \
	SLATE_BEGIN_ARGS(Type) \
			: _Padding(16.0f) \
			, _IconSize(24,24) \
			, _AutoWrapText(true) \
			, _Content() \
			{} \
	SLATE_ARGUMENT(FMargin, Padding) \
	SLATE_ARGUMENT(FVector2D, IconSize) \
	SLATE_ARGUMENT(bool, AutoWrapText) \
	SLATE_DEFAULT_SLOT(FArguments, Content)

class STextBlock;
class SImage;

enum class EStatusBoxState : uint8
{
	Success,
	Warning,
	Error
};

class SStatusBox : public SBorder
{
public:
	SLATE_STATUS_BOX_ARGS(SStatusBox)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	void SetStatus(EStatusBoxState NewStatus, bool bInForceIfSame);

protected:
	virtual void OnStatusChanged(EStatusBoxState NewStatus);
	virtual const FSlateBrush* GetBorderBrush(EStatusBoxState ForStatus) const;
	virtual const FSlateBrush* GetImageBrush(EStatusBoxState ForStatus) const;
	virtual FText GetStatusText(EStatusBoxState ForStatus) const;
	virtual FSlateColor StatusToColor(EStatusBoxState Status) const;
	
	TSharedPtr<STextBlock> StatusText;
	TSharedPtr<SImage> StatusImage;
	
private:
	EStatusBoxState BoxStatus = EStatusBoxState::Error;
	FSlateBrush BorderBrush;
};