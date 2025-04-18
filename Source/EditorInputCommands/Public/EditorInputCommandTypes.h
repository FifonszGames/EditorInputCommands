﻿// Copyright FifonszGames 2025 All Rights Reserved.

#pragma once

#include "Framework/Commands/UICommandInfo.h"
#include "Framework/Commands/InputChord.h"
#include "Templates/SharedPointer.h"
#include "EditorInputCommandTypes.generated.h"

class FUICommandList;

USTRUCT(NotBlueprintType)
struct FBindingContextProvider
{
	GENERATED_BODY()
	
	virtual ~FBindingContextProvider() = default;

	bool IsValid() const { return AsContext().IsValid(); }
	
	virtual FName GetBindingContextName() const PURE_VIRTUAL(FBindingContextProvider::GetBindingContextName, return NAME_None;);
	virtual TSharedPtr<FBindingContext> AsContext() const;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "EditorInputCommandsLibrary.MakeCommandIdentifier"))
struct FExistingContextBinding : public FBindingContextProvider
{
	GENERATED_BODY()
	explicit FExistingContextBinding() = default;
	explicit FExistingContextBinding(const FName InBindingContextName) : BindingContextName(InBindingContextName) {}

	virtual FName GetBindingContextName() const override { return BindingContextName; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands", meta=(GetOptions="EditorInputCommandsLibrary.GetBindingContextNames"))
	FName BindingContextName;
};

UENUM(BlueprintType)
enum class ECommandRepeatMode : uint8
{
	Disabled,
	Enabled
};

struct FCommandIdentifier;

USTRUCT(BlueprintType)
struct FInputCommandRegisterData
{
	GENERATED_BODY()

	explicit FInputCommandRegisterData() = default;
	explicit FInputCommandRegisterData(const TSharedRef<FBindingContext>& Context, const TSharedRef<FUICommandInfo>& CommandInfo);
	
	bool IsValid() const;
	
	TSharedPtr<FBindingContext> GetContext() const;
	FCommandIdentifier GetIdentifier() const;

	static FSlateIcon GetIcon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands", meta=(ShowOnlyInnerProperties))
	FExistingContextBinding ContextProvider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands")
	FName Identifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands")
	FText Label;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands")
	FInputChord DefaultChord;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands")
	FInputChord AlternateDefaultChord;

	EUserInterfaceActionType UIActionType = EUserInterfaceActionType::None;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "EditorInputCommandsLibrary.MakeCommandIdentifier"))
struct FCommandIdentifier
{
	GENERATED_BODY()
	explicit FCommandIdentifier() = default;
	explicit FCommandIdentifier(const FName& InBindingContext, const FName& InCommandName) : BindingContext(InBindingContext), Identifier(InCommandName) {}
	explicit FCommandIdentifier(const FInputCommandRegisterData& Data);

	bool operator==(const FCommandIdentifier& Other) const { return BindingContext.IsEqual(Other.BindingContext) && Identifier.IsEqual(Other.Identifier); }
	bool operator!=(const FCommandIdentifier& Other) const { return !(*this == Other); }

	TSharedPtr<FUICommandInfo> AsInfo() const;
	TSharedPtr<FBindingContext> AsContext() const;
	
	bool CanEverPointToValidCommand() const;
	bool IsRegistered() const;

	bool GetUserDefinedChord(const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands", meta=(GetOptions="EditorInputCommandsLibrary.GetBindingContextNames"))
	FName BindingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands", meta=(GetOptions="EditorInputCommandsLibrary.GetCommandNames"))
	FName Identifier;

	friend class FInputCommandIdentifierCustomization;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "EditorInputCommandsLibrary.MakeCommandListIdentifier"))
struct FCommandListIdentifier
{
	GENERATED_BODY()
	
	friend uint32 GetTypeHash(const FCommandListIdentifier& InIdentifier);
	
	explicit FCommandListIdentifier() = default;
	explicit FCommandListIdentifier(const FName Identifier): Identifier(Identifier) {}

	bool operator==(const FCommandListIdentifier& Other) const { return Identifier == Other.Identifier; }
	bool operator!=(const FCommandListIdentifier& Other) const { return !(*this == Other); }
	
	bool operator==(const FName& Other) const { return Identifier == Other; }
	bool operator!=(const FName& Other) const { return !(*this == Other); }

	bool ForeachCommandList(const TFunctionRef<bool(const TSharedRef<FUICommandList>& List)>& InFunc) const;
	bool IsValid() const;
	
protected:
	const TArray<TWeakPtr<FUICommandList>>* GetRelatedCommandLists() const;
	bool HasValidCommandList() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Editor Input Commands", meta=(GetOptions="EditorInputCommandsLibrary.GetCommandListIdentifiers"))
	FName Identifier;

	friend class FCommandListIdentifierSpec;
};

inline uint32 GetTypeHash(const FCommandListIdentifier& ListIdentifier)
{
	return GetTypeHash(ListIdentifier.Identifier);
}
