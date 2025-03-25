// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once
#include "KismetPins/SGraphPinString.h"

class SSearchableComboBox;

class SCommandIdentifierPin : public SGraphPinString
{
public:
	SLATE_BEGIN_ARGS(SCommandIdentifierPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface
	
private:
	void OnComboBoxOpening();
	
	TSharedRef<SWidget> CreateComboBoxEntry(TSharedPtr<FString> Value);
	void OnComboBoxSelectionChanged(TSharedPtr<FString> Value, ESelectInfo::Type InSelectInfo);

	TArray<TSharedPtr<FString>> IdentifierOptions;

	TSharedPtr<SSearchableComboBox> SearchableBox;

	TOptional<FString> CurrentContextValue;
};
