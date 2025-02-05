// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/EditorCommandStyle.h"

#include "CommandsExtension.h"
#include "EditorInputCommand.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH(RelativePath,...) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedPtr< FSlateStyleSet > FEditorCommandStyle::StyleInstance = nullptr;

void FEditorCommandStyle::Init()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = CreateStyle();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FEditorCommandStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
}

const ISlateStyle& FEditorCommandStyle::Get()
{
	check(StyleInstance.IsValid());
	return *StyleInstance;
}

FName FEditorCommandStyle::GetOutlineStyleName()
{
	return TEXT("RoundedOutline");
}

TSharedRef<FSlateStyleSet> FEditorCommandStyle::CreateStyle()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("EditorCommandStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin(FCommandsExtensionModule::GetModuleName().ToString())->GetBaseDir() / TEXT("Resources"));

	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);
	
	Style->Set(GetClassIconStyleSetName<UEditorInputCommand>(), new IMAGE_BRUSH("Icon128", Icon16x16));
	Style->Set(GetClassThumbnailStyleSetName<UEditorInputCommand>(), new IMAGE_BRUSH("Icon128", Icon64x64));
	Style->Set(GetOutlineStyleName(), new FSlateRoundedBoxBrush(FStyleColors::Transparent, 4.0f, FStyleColors::White, 1.0f));
	return Style;
}

#undef IMAGE_BRUSH
