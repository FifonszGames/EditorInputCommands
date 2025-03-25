// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

struct FCommandIdentifier;

class FInputCommandIdentifierCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	bool IsContextNotNone() const;
	
	const FCommandIdentifier* GetIdentifier() const;
	TSharedPtr<IPropertyHandle> StructHandle;
};