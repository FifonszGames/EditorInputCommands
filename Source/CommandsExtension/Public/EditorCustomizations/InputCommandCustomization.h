// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"


class SEditorCommandMappingStatusBox;
class IDetailLayoutBuilder;
class SEditorCommandRegistrationStatusBox;
class IPropertyUtilities;
class UEditorInputCommand;

typedef TFunctionRef<bool(const UEditorInputCommand& Command)> IsEnabledCallback;

class FInputCommandCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& InDetailLayout) override;
	
private:
	void OnFinishChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent) const;
	TSharedRef<SButton> CreateButton(const FText& Name, const TFunctionRef<void(UEditorInputCommand& Command)>& OnClicked, const IsEnabledCallback& IsEnabled);
	
	TWeakPtr<IPropertyUtilities> Utilities;
	TSharedPtr<SEditorCommandRegistrationStatusBox> RegistrationStatusBox;
	TSharedPtr<SEditorCommandMappingStatusBox> MappingStatusBox;

	TMap<TSharedPtr<SButton>, IsEnabledCallback> ButtonsMap;
	TWeakObjectPtr<UEditorInputCommand> Target;
};