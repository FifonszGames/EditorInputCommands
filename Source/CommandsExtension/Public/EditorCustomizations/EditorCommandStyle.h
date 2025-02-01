// Copyright FifonszGames. All Rights Reserved.

#pragma once

class FSlateStyleSet;
class ISlateStyle;

class FEditorCommandStyle
{
public:
	static void Init();
	static void Shutdown();
	static const ISlateStyle& Get();
	
	template<typename ClassType>
	static FName GetClassIconStyleSetName()
	{
		const FString ClassName = ClassType::StaticClass()->GetName();
		return FName(FString::Printf(TEXT("ClassIcon.%s"), *ClassName));
	}
	template<typename ClassType>
	static FName GetClassThumbnailStyleSetName()
	{
		const FString ClassName = ClassType::StaticClass()->GetName();
		return FName(FString::Printf(TEXT("ClassThumbnail.%s"), *ClassName));
	}
	
private:
	static TSharedRef<FSlateStyleSet> CreateStyle();
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
