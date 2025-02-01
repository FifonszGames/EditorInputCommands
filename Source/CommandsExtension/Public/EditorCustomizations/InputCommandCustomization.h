// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class SEditorCommandStatusBox;

class FInputCommandCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& InDetailLayout) override;
	
private:
	void OnPropertyValueChanged() const;
	TSharedPtr<SEditorCommandStatusBox> StatusBox;
};