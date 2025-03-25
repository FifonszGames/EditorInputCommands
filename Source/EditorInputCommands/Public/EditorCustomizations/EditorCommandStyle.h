// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once

class FSlateStyleSet;
class ISlateStyle;

class FEditorCommandStyle
{
public:
	static void Init();
	static void Shutdown();
	static const ISlateStyle& Get();
	static FName GetOutlineStyleName();
	
	template <typename T>
	static typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, FName>::Type GetClassIconStyleSetName()
	{
		const FString ClassName = T::StaticClass()->GetName();
		return FName(FString::Printf(TEXT("ClassIcon.%s"), *ClassName));
	}
	template <typename T>
	static typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, FName>::Type GetClassThumbnailStyleSetName()
	{
		const FString ClassName = T::StaticClass()->GetName();
		return FName(FString::Printf(TEXT("ClassThumbnail.%s"), *ClassName));
	}
	
private:
	static TSharedRef<FSlateStyleSet> CreateStyle();
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
