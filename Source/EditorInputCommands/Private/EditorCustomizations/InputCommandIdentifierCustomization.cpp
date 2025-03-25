// Copyright FifonszGames 2025 All Rights Reserved.


#include "EditorCustomizations/InputCommandIdentifierCustomization.h"

#include "EditorInputCommandsLibrary.h"
#include "EditorInputCommandTypes.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Framework/Commands/InputBindingManager.h"

TSharedRef<IPropertyTypeCustomization> FInputCommandIdentifierCustomization::MakeInstance()
{
	return MakeShared<FInputCommandIdentifierCustomization>();
}

void FInputCommandIdentifierCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	StructHandle = PropertyHandle;
	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		PropertyHandle->CreatePropertyValueWidget()
	];
}

void FInputCommandIdentifierCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedRef<IPropertyHandle> ContextHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCommandIdentifier, BindingContext)).ToSharedRef();
	FPropertyComboBoxArgs BindingContextArgs(ContextHandle, FOnGetPropertyComboBoxStrings::CreateLambda(
	[this](TArray<TSharedPtr<FString>>& OutComboBoxStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems)  
	{
		UEditorInputCommandsLibrary::ForeachBindingContext([&OutComboBoxStrings](const TSharedPtr<FBindingContext>& Context)
		{
			OutComboBoxStrings.AddUnique(MakeShared<FString>(Context->GetContextName().ToString()));
		});
	}),
	FOnGetPropertyComboBoxValue(),
	FOnPropertyComboBoxValueSelected::CreateLambda([this, ContextHandle](const FString& SelectedValue)
	{
		FString PreviousValue;
		ContextHandle->GetValue(PreviousValue);
		ContextHandle->SetValueFromFormattedString(SelectedValue);
		if (PreviousValue != SelectedValue)
		{
			StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCommandIdentifier, Identifier))->SetValue(NAME_None);
		}
	}));
	
	ChildBuilder.AddProperty(ContextHandle).CustomWidget()
	.NameContent()
	[
		ContextHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		PropertyCustomizationHelpers::MakePropertyComboBox(BindingContextArgs)
	];

	TSharedRef<IPropertyHandle> IdentifierHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCommandIdentifier, Identifier)).ToSharedRef();
	FPropertyComboBoxArgs IdentifierArgs(IdentifierHandle, FOnGetPropertyComboBoxStrings::CreateLambda(
[this](TArray<TSharedPtr<FString>>& OutComboBoxStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems)  
	{
		if (const FCommandIdentifier* Identifier = GetIdentifier())
		{
			FInputBindingManager& Manager = FInputBindingManager::Get();
			TSharedPtr<FBindingContext> Context = Manager.GetContextByName(Identifier->BindingContext);
			if (Context.IsValid())
			{
				UEditorInputCommandsLibrary::ForeachCommandInContext(Context->GetContextName(), [&OutComboBoxStrings](const TSharedPtr<FUICommandInfo>& Command)
				{
					OutComboBoxStrings.AddUnique(MakeShared<FString>(Command->GetCommandName().ToString()));
				});
			}
		}
	}));
	IDetailPropertyRow& IdentifierRow = ChildBuilder.AddProperty(IdentifierHandle);
	IdentifierRow.IsEnabled(TAttribute<bool>(this, &FInputCommandIdentifierCustomization::IsContextNotNone));
	IdentifierRow.CustomWidget().NameContent()
	[
		IdentifierHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		PropertyCustomizationHelpers::MakePropertyComboBox(IdentifierArgs)
	];
}

bool FInputCommandIdentifierCustomization::IsContextNotNone() const
{
	const FCommandIdentifier* Identifier = GetIdentifier();
	return Identifier && !Identifier->BindingContext.IsNone();
}

const FCommandIdentifier* FInputCommandIdentifierCustomization::GetIdentifier() const
{
	if (StructHandle.IsValid())
	{
		void* RawData = nullptr;
		FPropertyAccess::Result Result = StructHandle->GetValueData(RawData);
		if (Result == FPropertyAccess::Success)
		{
			return static_cast<FCommandIdentifier*>(RawData);
		}
	}
	return nullptr;	
}
