// Copyright FifonszGames. All Rights Reserved.

#include "EditorCustomizations/InputCommandCustomization.h"

#include "CommandsExtensionLibrary.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorInputCommand.h"
#include "IPropertyUtilities.h"
#include "EditorCustomizations/EditorCommandRegistrationStatusBox.h"

	TSharedRef<SButton> FInputCommandCustomization::CreateButton(const FText& Name, TWeakObjectPtr<UEditorInputCommand> CommandTarget, const TFunctionRef<void(UEditorInputCommand& Command)>& OnClicked, const TFunctionRef<bool(const UEditorInputCommand& Command)>& IsEnabled) const
	{
		return SNew(SButton)
			.Text(Name)
			.OnClicked_Lambda([Target = CommandTarget, OnClicked, this]()
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
			.IsEnabled_Lambda([Target = CommandTarget, IsEnabled]
			{
				const UEditorInputCommand* Command = Target.Get();
				return Command && IsEnabled(*Command);
			});
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
	
	TWeakObjectPtr Target = Cast<UEditorInputCommand>(SelectedObjects[0]);
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
		.Padding(FMargin(0.f, 16.f, 0.f, 0.f))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				CreateButton(TextFromString("Register Command"), Target,
											[](UEditorInputCommand& Command) { Command.RegisterCommand(); },
											[](const UEditorInputCommand& Command) { return Command.RegistrationData.IsValid() &&
												(Command.RegistrationData.GetIdentifier() != Command.CurrentIdentifier || !Command.CurrentIdentifier.IsRegistered()); })
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				CreateButton(TextFromString("Unregister Command"), Target,
					[](UEditorInputCommand& Command) { Command.UnregisterCommand(); },
					[](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered(); })
			]	
		]
	];

	const FName MappingCategoryName = TEXT("Mapping");
	IDetailCategoryBuilder& MappingBuilder = InDetailLayout.EditCategory(MappingCategoryName);
	MappingBuilder.AddCustomRow(FText::FromName(MappingCategoryName))
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			CreateButton(TextFromString("Map to selected list"), Target,
			                            [](UEditorInputCommand& Command) { Command.MapToTargetList(); },
			                            [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered() && !Command.MappedLists.Contains(Command.TargetList); })
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			CreateButton(TextFromString("Unmap from selected list"), Target,
			                            [](UEditorInputCommand& Command) { Command.UnmapFromTargetList(); },
			                            [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered() && Command.MappedLists.Contains(Command.TargetList); })
		]
	];
}

void FInputCommandCustomization::OnFinishChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (RegistrationStatusBox.IsValid())
	{
		RegistrationStatusBox->RefreshState();
	}	
}

#undef TextFromString
