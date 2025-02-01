// Copyright FifonszGames. All Rights Reserved.

#include "EditorCustomizations/InputCommandCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorInputCommand.h"
#include "EditorCustomizations/EditorCommandStatusBox.h"

#define TextFromString(RawString) FText::FromString(TEXT(RawString))

namespace ButtonHelpers
{
	TSharedRef<SButton> CreateButton(const FText& Name, TWeakObjectPtr<UEditorInputCommand> CommandTarget, const TFunctionRef<void(UEditorInputCommand& Command)>& OnClicked, const TFunctionRef<bool(const UEditorInputCommand& Command)>& IsEnabled)
	{
		return SNew(SButton)
			.Text(Name)
			.OnClicked_Lambda([Target = CommandTarget, OnClicked]()
			{
				if (UEditorInputCommand* Command = Target.Get())
				{
					OnClicked(*Command);
				} 
				return FReply::Handled();
			})
			.IsEnabled_Lambda([Target = CommandTarget, IsEnabled]
			{
				const UEditorInputCommand* Command = Target.Get();
				return Command && IsEnabled(*Command);
			});
		}
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
	TSharedRef<IPropertyHandle> Handle = InDetailLayout.GetProperty(InDetailLayout.GetTopLevelProperty());
	IDetailCategoryBuilder& StatusBuilder = InDetailLayout.EditCategory(TEXT("AStatus"));
	StatusBuilder.AddCustomRow(TextFromString("Status"))
	[
		SNew(SEditorCommandStatusBox)
		.CommandHandle(Handle)
	];
	
	TWeakObjectPtr Target = Cast<UEditorInputCommand>(SelectedObjects[0]);
	const FName RegistrationCategoryName = TEXT("Registration");
	IDetailCategoryBuilder& CategoryBuilder = InDetailLayout.EditCategory(RegistrationCategoryName);
	CategoryBuilder.AddCustomRow(FText::FromName(RegistrationCategoryName))
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			ButtonHelpers::CreateButton(TextFromString("Register Command"), Target,
			                            [](UEditorInputCommand& Command) { Command.RegisterCommand(); },
			                            [](const UEditorInputCommand& Command) { return Command.RegistrationData.IsValid(); })
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			ButtonHelpers::CreateButton(TextFromString("Unregister Command"), Target,
				[](UEditorInputCommand& Command) { Command.UnregisterCommand(); },
				[](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered(); })
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
			ButtonHelpers::CreateButton(TextFromString("Map to selected list"), Target,
			                            [](UEditorInputCommand& Command) { Command.MapToTargetList(); },
			                            [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered() && !Command.MappedLists.Contains(Command.TargetList); })
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			ButtonHelpers::CreateButton(TextFromString("Unmap from selected list"), Target,
			                            [](UEditorInputCommand& Command) { Command.UnmapFromTargetList(); },
			                            [](const UEditorInputCommand& Command) { return Command.CurrentIdentifier.IsRegistered() && Command.MappedLists.Contains(Command.TargetList); })
		]
	];
}

#undef TextFromString