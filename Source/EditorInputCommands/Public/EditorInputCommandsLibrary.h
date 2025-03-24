// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "EditorInputCommandsSubsystem.h"
#include "EditorInputCommandTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorInputCommandsLibrary.generated.h"

class UEditorInputCommand;

#define STRING_TO_TEXT(RawString) FText::FromString(TEXT(RawString))

USTRUCT(BlueprintType)
struct FCommandMappingData
{
	GENERATED_BODY()
	
	explicit FCommandMappingData() = default;
	explicit FCommandMappingData(const FCommandListIdentifier& InTargetList, const FCommandIdentifier& InCommandIdentifier)
		: CommandIdentifier(InCommandIdentifier), TargetList(InTargetList) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCommandIdentifier CommandIdentifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCommandListIdentifier TargetList;
};

UENUM(BlueprintType)
enum class ERegistrationResult : uint8
{
	Success,
	AlreadyRegistered,
	InvalidRegistrationData
};

UCLASS()
class EDITORINPUTCOMMANDS_API UEditorInputCommandsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="InData", ExpandEnumAsExecs="OutResult"))
	static void RegisterInputCommand(const FInputCommandRegisterData& InData, ERegistrationResult& OutResult);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="CommandIdentifier"))
	static bool UnregisterInputCommand(const FCommandIdentifier& CommandIdentifier);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="InMappingData"))
	static bool MapAction(const FCommandMappingData& InMappingData, FOnExecute OnExecute, const bool bInOverrideIfAlreadyMapped = true,
	                      ECommandRepeatMode RepeatMode = ECommandRepeatMode::Disabled);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	static bool MapActionToCommand(UEditorInputCommand* CommandAsset, const FOnExecute& OnExecute);
		
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="InMappingData"))
	static bool UnmapAction(const FCommandMappingData& InMappingData);
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	static bool UnmapActionFromCommand(UEditorInputCommand* CommandAsset, const FOnExecute& OnExecute);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="CommandIdentifier"))
	static bool GetUserDefinedCommandInputChord(const FCommandIdentifier& CommandIdentifier, const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="CommandIdentifier"))
	static bool IsCommandRegistered(const FCommandIdentifier& CommandIdentifier);

	UFUNCTION(BlueprintPure, meta=(NativeMakeFunc))
	static FCommandIdentifier MakeCommandIdentifier(UPARAM(Meta=(GetOptions=GetBindingContextNames)) const FName BindingContext,
		UPARAM(Meta=(GetOptions=GetCommandNames)) const FName Identifier);

	UFUNCTION(BlueprintPure, meta=(NativeMakeFunc))
	static FExistingContextBinding MakeExistingContextBinding(UPARAM(Meta=(GetOptions=GetBindingContextNames)) const FName BindingContextName);

	UFUNCTION(BlueprintPure, meta=(NativeMakeFunc))
	static FCommandListIdentifier MakeCommandListIdentifier(UPARAM(Meta=(GetOptions=GetCommandListIdentifiers)) const FName ListIdentifier);
	
	static void ForeachBindingContext(const TFunctionRef<void(const TSharedPtr<FBindingContext>& Context)>& InFunc);
	static void ForeachCommandInContext(const FName& ContextName, const TFunctionRef<void(const TSharedPtr<FUICommandInfo>& Command)>& InFunc);
	
private:
	UFUNCTION()
	static TArray<FName> GetBindingContextNames();
	UFUNCTION()
	static TArray<FName> GetCommandNames();
	UFUNCTION()
	static TArray<FName> GetCommandListIdentifiers();

	friend class FEditorInputCommandsLibrarySpec;
};
