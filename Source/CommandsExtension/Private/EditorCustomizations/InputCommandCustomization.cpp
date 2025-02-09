// Copyright FifonszGames. All Rights Reserved.

#include "EditorCustomizations/InputCommandCustomization.h"

#include "CommandsExtensionLibrary.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorInputCommand.h"
#include "IPropertyUtilities.h"
#include "EditorCustomizations/EditorCommandMappingStatusBox.h"
#include "EditorCustomizations/EditorCommandRegistrationStatusBox.h"

TSharedRef<SButton> FInputCommandCustomization::CreateButton(const FText& Name, const TFunctionRef<void(UEditorInputCommand& Command)>& OnClicked, const IsEnabledCallback& IsEnabled)
{
	const UEditorInputCommand* Command = Target.Get();
	
	TSharedRef<SButton> Button =  SNew(SButton)
	.Text(Name)
	.OnClicked_Lambda([OnClicked, this]()
	{
		if (UEditorInputCommand* Command = Target.Get())
		{
			OnClicked(*Command);
			if (Utilities.IsValid())
			{
				Utilities.Pin()->NotifyFinishedChangingProperties(FPropertyChangedEvent(nullptr));
			}
		} 
		return FReply::Handled();
	})
	.IsEnabled(Command && IsEnabled(*Command));
	
	ButtonsMap.Add(Button.ToSharedPtr(), IsEnabled);
	return Button;
}

TSharedRef<IDetailCustomization> FInputCommandCustomization::MakeInstance()
{
	return MakeShared<FInputCommandCustomization>();
}

void FInputCommandCustomization::CustomizeDetails(IDetailLayoutBuilder& InDetailLayout)
	{
		const TArray<TWeakObjectPtr<UObject>>& SelectedObjects = InDetailLayout.GetSelectedObjects();
		if (SelectedObjects.Num() != 1)
		{
			return;
		}
		IDetailsView* DetailsView = InDetailLayout.GetDetailsView();
		check(DetailsView);
		DetailsView->OnFinishedChangingProperties().AddRaw(this, &FInputCommandCustomization::OnFinishChangingProperties);
	
		Utilities = InDetailLayout.GetPropertyUtilities();
	
		Target = Cast<UEditorInputCommand>(SelectedObjects[0]);
		const FMargin BoxPadding(0.f, 16.f, 0.f, 0.f);
		
		const FName RegistrationCategoryName = TEXT("Registration");
		IDetailCategoryBuilder& CategoryBuilder = InDetailLayout.EditCategory(RegistrationCategoryName);
		CategoryBuilder.AddCustomRow(FText::FromName(RegistrationCategoryName))
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(RegistrationStatusBox, SEditorCommandRegistrationStatusBox)
				.TargetCommand(Target)
			]
			+SVerticalBox::Slot()
			.Padding(BoxPadding)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					CreateButton(STRING_TO_TEXT("Register Command"), [](UEditorInputCommand& Command) { Command.RegisterCommand(); },
					             [](const UEditorInputCommand& Command) { return Command.RegistrationData.IsValid() &&
						             (Command.RegistrationData.GetIdentifier() != Command.CurrentIdentifier || !Command.CurrentIdentifier.IsRegistered()); })
				]
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					CreateButton(STRING_TO_TEXT("Unregister Command"), [](UEditorInputCommand& Command) { Command.UnregisterCommand(); },
					             [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered(); })
				]	
			]
		];

		const FName MappingCategoryName = TEXT("Mapping");
		IDetailCategoryBuilder& MappingBuilder = InDetailLayout.EditCategory(MappingCategoryName);
		MappingBuilder.AddCustomRow(FText::FromName(MappingCategoryName))
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(MappingStatusBox, SEditorCommandMappingStatusBox)
				.TargetCommand(Target)
			]
			+SVerticalBox::Slot()
			.Padding(BoxPadding)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					CreateButton(STRING_TO_TEXT("Map to selected list"), [](UEditorInputCommand& Command) { Command.MapToTargetList(); },
					             [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered() && !Command.MappedLists.Contains(Command.TargetList); })
					]
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					CreateButton(STRING_TO_TEXT("Unmap from selected list"), [](UEditorInputCommand& Command) { Command.UnmapFromTargetList(); },
					             [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered() && Command.MappedLists.Contains(Command.TargetList); })
				]
			]
	];
}

void FInputCommandCustomization::OnFinishChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent) const
{
	if (RegistrationStatusBox.IsValid())
	{
		RegistrationStatusBox->RefreshState();
	}
	if (MappingStatusBox.IsValid())
	{
		MappingStatusBox->RefreshState();
	}
	if (const UEditorInputCommand* Command = Target.Get())
	{
		for (const auto& [Button, Callback] : ButtonsMap)
		{
			Button->SetEnabled(Callback(*Command));
		}
	}
}

#undef STRING_TO_TEXT
