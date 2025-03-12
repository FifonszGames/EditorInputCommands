// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/CommandIdentifierPin.h"

#include "EditorInputCommandsLibrary.h"
#include "SSearchableComboBox.h"

void SCommandIdentifierPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPinString::Construct(SGraphPinString::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SCommandIdentifierPin::GetDefaultValueWidget()
{
	return SNew(SBox)
		.MinDesiredWidth(18)
		.MaxDesiredWidth(400)
		[
			SAssignNew(SearchableBox, SSearchableComboBox)
			.OptionsSource(&IdentifierOptions)
			.OnComboBoxOpening(FOnComboBoxOpening::CreateSP(this, &SCommandIdentifierPin::OnComboBoxOpening))
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsEnabled(this, &SGraphPin::GetDefaultValueIsEditable)
			.OnGenerateWidget(this, &SCommandIdentifierPin::CreateComboBoxEntry)
			.OnSelectionChanged(this, &SCommandIdentifierPin::OnComboBoxSelectionChanged)
			.ContentPadding(3.0f)
			.Content()
			[
				SNew(STextBlock)
					.Text(this, &SCommandIdentifierPin::GetTypeInValue)
					.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			]
		];
}

void SCommandIdentifierPin::OnComboBoxOpening()
{
	if (UEdGraphPin* IdentifierPin = GetPinObj())
	{
		if (UEdGraphNode* OwningNode = IdentifierPin->GetOwningNode())
		{
			const FName ContextInputName = TEXT("BindingContext");
			UEdGraphPin** ContextPinPtr = OwningNode->Pins.FindByPredicate([ContextInputName](const UEdGraphPin* ChildPin)
			{
				return ChildPin && ChildPin->Direction == EGPD_Input && ChildPin->PinName.IsEqual(ContextInputName);
			});
			if (ContextPinPtr)
			{
				const FString ContextValue = (*ContextPinPtr)->GetDefaultAsString();
				if (!CurrentContextValue.IsSet() || CurrentContextValue.GetValue().Equals(ContextValue))
				{
					IdentifierOptions.Empty();
					UEditorInputCommandsLibrary::ForeachCommandInContext(FName(ContextValue), [this](const TSharedPtr<FUICommandInfo>& Command)
					{
						IdentifierOptions.AddUnique(MakeShared<FString>(Command->GetCommandName().ToString()));
					});
					SearchableBox->RefreshOptions();
				}
				return;
			}
		}
	}
	IdentifierOptions.Empty();
	SearchableBox->RefreshOptions();
}

TSharedRef<SWidget> SCommandIdentifierPin::CreateComboBoxEntry(TSharedPtr<FString> Value)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*Value))
		.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"));
}

void SCommandIdentifierPin::OnComboBoxSelectionChanged(TSharedPtr<FString> Value, ESelectInfo::Type InSelectInfo)
{
	if (GraphPinObj->IsPendingKill())
	{
		return;
	}

	if (!GraphPinObj->GetDefaultAsString().Equals(*Value))
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeStringPinValue", "Change String Pin Value"));
		GraphPinObj->Modify();
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *Value);
	}
}
