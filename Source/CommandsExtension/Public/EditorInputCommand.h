// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CommandExtensionSubsystem.h"
#include "EditorUtilityObject.h"
#include "CommandExtensionTypes.h"
#include "EditorInputCommand.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputCommandExecuted);

UCLASS(Abstract)
class UEditorInputCommand : public UEditorUtilityObject
{
	GENERATED_BODY()

public:
	virtual bool IsEditorOnly() const override final { return true; }
	
	void RegisterCommand();
	void UnregisterCommand();

	void MapToTargetList();
	void UnmapFromTargetList();

	UPROPERTY(BlueprintAssignable, Transient)
	FOnInputCommandExecuted OnInputCommandExecuted;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Registration")
	FInputCommandRegisterData RegistrationData;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Registration", NoClear)
	FCommandIdentifier CurrentIdentifier;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mapping")
	FCommandListIdentifier TargetList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mapping")
	ECommandRepeatMode RepeatMode = ECommandRepeatMode::Disabled;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Mapping", NoClear, meta=(ShowOnlyInnerProperties))
	TSet<FCommandListIdentifier> MappedLists;

private:
	UFUNCTION()
	void ExecuteCommand();
	
	FOnExecute ExecuteDelegate;
	
	friend class FInputCommandCustomization;
	friend class SEditorCommandRegistrationStatusBox;
	friend class SEditorCommandMappingStatusBox;
};
