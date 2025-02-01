// Copyright FifonszGames. All Rights Reserved.

#pragma once
#include "StructUtils/InstancedStruct.h"

#include "CommandExtensionTypes.generated.h"

USTRUCT(NotBlueprintType)
struct FBindingContextProvider
{
	GENERATED_BODY()
	
	virtual ~FBindingContextProvider() = default;

	bool IsValid() const { return AsContext().IsValid(); }
	
	virtual FName GetBindingContextName() const { return NAME_None; }
	virtual TSharedPtr<FBindingContext> AsContext() const;
};

USTRUCT(BlueprintType)
struct FExistingContextBinding : public FBindingContextProvider
{
	GENERATED_BODY()
	
	virtual FName GetBindingContextName() const override { return BindingContextName; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GetOptions="CommandsExtensionLibrary.GetBindingContextNames"))
	FName BindingContextName;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "CommandsExtensionLibrary.MakeNewContextBinding"))
struct FNewContextBinding : public FBindingContextProvider
{
	GENERATED_BODY()
	explicit FNewContextBinding() = default;
	explicit FNewContextBinding(const FName BindingContextName, const FText& InContextDescription);

	virtual FName GetBindingContextName() const override { return BindingContextName; }
	virtual TSharedPtr<FBindingContext> AsContext() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BindingContextName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ContextDescription;

private:
	void CreateContext() const;
	
	mutable TSharedPtr<FBindingContext> Context;
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

	explicit FInputCommandRegisterData();
	
	bool IsValid() const;
	
	TSharedPtr<FBindingContext> GetContext() const;
	FCommandIdentifier GetIdentifier() const;

	static FSlateIcon GetIcon();

	UPROPERTY(EditAnywhere, NoClear, meta = (ExcludeBaseStruct, BaseStruct="/Script/CommandsExtension.BindingContextProvider"))
	FInstancedStruct ContextProvider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Identifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Label;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInputChord DefaultChord;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInputChord AlternateDefaultChord;

	EUserInterfaceActionType UIActionType = EUserInterfaceActionType::None;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "CommandsExtensionLibrary.MakeCommandIdentifier"))
struct FCommandIdentifier
{
	GENERATED_BODY()
	explicit FCommandIdentifier() = default;
	explicit FCommandIdentifier(const FName& InBindingContext, const FName& InCommandName) : BindingContext(InBindingContext), Identifier(InCommandName) {}
	explicit FCommandIdentifier(const FInputCommandRegisterData& Data);

	TSharedPtr<FUICommandInfo> AsInfo() const;
	TSharedPtr<FBindingContext> AsContext() const;
	
	bool CanEverPointToValidCommand() const;
	bool IsRegistered() const;

	bool GetUserDefinedChord(const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GetOptions="CommandsExtensionLibrary.GetBindingContextNames"))
	FName BindingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GetOptions="CommandsExtensionLibrary.GetCommandNames"))
	FName Identifier;

	friend class FInputCommandIdentifierCustomization;
};

USTRUCT(BlueprintType)
struct FCommandListIdentifier
{
	GENERATED_BODY()
	
	friend uint32 GetTypeHash(const FCommandListIdentifier& InIdentifier);
	
	explicit FCommandListIdentifier() = default;
	explicit FCommandListIdentifier(const FName Identifier): Identifier(Identifier) {}

	bool operator==(const FCommandListIdentifier& Other) const { return Identifier == Other.Identifier; }
	bool operator!=(const FCommandListIdentifier& Other) const { return !(*this == Other); }

	TWeakPtr<FUICommandList> AsCommandList() const;
	bool IsValid() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GetOptions="CommandsExtensionLibrary.GetCommandListIdentifiers"))
	FName Identifier;
};

inline uint32 GetTypeHash(const FCommandListIdentifier& ListIdentifier)
{
	return GetTypeHash(ListIdentifier.Identifier);
}

