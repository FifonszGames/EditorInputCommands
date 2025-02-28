// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CommandExtensionSubsystem.h"
#include "Engine/DataAsset.h"
#include "CommandExtensionTypes.h"
#include "EditorInputCommand.generated.h"

class UEditorUtilityObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputCommandExecuted);

UCLASS()
class UEditorInputCommand : public UDataAsset
{
	GENERATED_BODY()

public:
	void RegisterCommand();
	void UnregisterCommand();

	void MapToTargetList();
	void UnmapFromTargetList();

	virtual void BeginDestroy() override;

	const FCommandListIdentifier& GetCommandListIdentifier() const { return TargetList; }

	UPROPERTY(BlueprintAssignable, Transient)
	FOnInputCommandExecuted OnInputCommandExecuted;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Registration")
	FInputCommandRegisterData RegistrationData;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Registration", NoClear, meta=(ShowOnlyInnerProperties, NoResetToDefault))
	FCommandIdentifier CurrentIdentifier;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mapping")
	FCommandListIdentifier TargetList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mapping")
	ECommandRepeatMode RepeatMode = ECommandRepeatMode::Disabled;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Mapping", NoClear, meta=(ShowOnlyInnerProperties, NoResetToDefault))
	TSet<FCommandListIdentifier> MappedLists;

	UPROPERTY(EditDefaultsOnly, Category="Execution")
	TSet<TSoftClassPtr<UEditorUtilityObject>> RunnableObjects;

private:
	UFUNCTION()
	void ExecuteCommand();

	UPROPERTY()
	FOnExecute ExecuteDelegate;
	
	friend class FInputCommandCustomization;
	friend class SEditorCommandRegistrationStatusBox;
	friend class SEditorCommandMappingStatusBox;
};
