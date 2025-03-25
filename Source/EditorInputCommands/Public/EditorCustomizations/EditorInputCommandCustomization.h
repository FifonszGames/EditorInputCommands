// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "Templates/Function.h"
#include "EditorInputCommand.h"
#include "UObject/WeakObjectPtrTemplates.h"

class SButton;
class SEditorCommandMappingStatusBox;
class IDetailLayoutBuilder;
class SEditorCommandRegistrationStatusBox;
class IPropertyUtilities;
struct FPropertyChangedEvent;

typedef TFunctionRef<bool(const UEditorInputCommand& Command)> IsEnabledCallback;

class FEditorInputCommandCustomization : public IDetailCustomization
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