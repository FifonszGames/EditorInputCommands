// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class SEditorCommandRegistrationStatusBox;
class IPropertyUtilities;
class UEditorInputCommand;

class FInputCommandCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& InDetailLayout) override;
	
private:
	void OnFinishChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	TSharedRef<SButton> CreateButton(const FText& Name, TWeakObjectPtr<UEditorInputCommand> CommandTarget, const TFunctionRef<void(UEditorInputCommand& Command)>& OnClicked, const TFunctionRef<bool(const UEditorInputCommand& Command)>& IsEnabled) const;
	TSharedPtr<SEditorCommandRegistrationStatusBox> RegistrationStatusBox;
	TWeakPtr<IPropertyUtilities> Utilities;
};