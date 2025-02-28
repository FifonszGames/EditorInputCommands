// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CommandExtensionSubsystem.h"
#include "CommandExtensionTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommandsExtensionLibrary.generated.h"

class UEditorInputCommand;

#define STRING_TO_TEXT(RawString) FText::FromString(TEXT(RawString))

UENUM(BlueprintType)
enum class ERegistrationResult : uint8
{
	Success,
	AlreadyRegistered,
	InvalidRegistrationData,
	InvalidContext
};

UCLASS()
class UCommandsExtensionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="InData", ExpandEnumAsExecs="OutResult"))
	static void RegisterInputCommand(const FInputCommandRegisterData& InData, ERegistrationResult& OutResult);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="CommandIdentifier"))
	static bool UnregisterInputCommand(const FCommandIdentifier& CommandIdentifier);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="TargetList, CommandIdentifier"))
	static bool MapAction(const FCommandListIdentifier& TargetList, const FCommandIdentifier& CommandIdentifier,
		FOnExecute OnExecute, ECommandRepeatMode RepeatMode = ECommandRepeatMode::Disabled);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	static bool MapActionToCommand(UEditorInputCommand* CommandAsset, FOnExecute OnExecute);
		
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="TargetList, CommandIdentifier"))
	static bool UnmapAction(const FCommandListIdentifier& TargetList, const FCommandIdentifier& CommandIdentifier, FOnExecute OnExecute);
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	static bool UnmapActionFromCommand(UEditorInputCommand* CommandAsset, FOnExecute OnExecute);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="CommandIdentifier"))
	static bool GetUserDefinedCommandInputChord(const FCommandIdentifier& CommandIdentifier, const EMultipleKeyBindingIndex InChordIndex, FInputChord& OutChord);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(AutoCreateRefTerm="CommandIdentifier"))
	static bool IsCommandRegistered(const FCommandIdentifier& CommandIdentifier);

	UFUNCTION(BlueprintPure, meta=(NativeMakeFunc))
	static FCommandIdentifier MakeCommandIdentifier(UPARAM(Meta=(GetOptions=GetBindingContextNames)) const FName BindingContext,
		UPARAM(Meta=(GetOptions=GetCommandNames)) const FName Identifier);

	UFUNCTION(BlueprintPure, meta=(NativeMakeFunc))
	static FExistingContextBinding MakeExistingContextBinding(UPARAM(Meta=(GetOptions=GetBindingContextNames)) const FName BindingContextName);
	
	UFUNCTION()
	static TArray<FName> GetBindingContextNames();
	
	static void ForeachBindingContext(const TFunctionRef<void(const TSharedPtr<FBindingContext>& Context)>& InFunc);
	static void ForeachCommandInContext(const FName& ContextName, const TFunctionRef<void(const TSharedPtr<FUICommandInfo>& Command)>& InFunc);
	
private:
	UFUNCTION()
	static TArray<FName> GetCommandNames();
	UFUNCTION()
	static TArray<FName> GetCommandListIdentifiers();
};
