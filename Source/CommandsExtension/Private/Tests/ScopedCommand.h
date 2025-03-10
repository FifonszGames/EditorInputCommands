// Copyright FifonszGames. All Rights Reserved.

#pragma once
#include "CommandExtensionTypes.h"

struct FScopedCommand
{
	explicit FScopedCommand(FInputBindingManager& InBindingManager, const FInputChord& DefaultChord = FInputChord());
	explicit FScopedCommand(const FInputChord& DefaultChord = FInputChord());
	~FScopedCommand();
		
	public:
		const FInputCommandRegisterData& GetData() const { return RegisterData; }
		FName GetBindingContextName() const;
	
	protected:
		FInputCommandRegisterData RegisterData;
		TSharedPtr<FBindingContext> BindingContext;
		TSharedPtr<FUICommandInfo> CommandInfo;
	
		FInputBindingManager* BindingManager;
		static uint16 ScopedCommandCount;
};

struct FScopedCommandWithList : public FScopedCommand
{
	explicit FScopedCommandWithList(FInputBindingManager& InBindingManager, const FInputChord& DefaultChord = FInputChord());
	explicit FScopedCommandWithList(const FInputChord& DefaultChord = FInputChord());
	~FScopedCommandWithList();

	TSharedPtr<FUICommandList> GetList() const { return DummyList;}
private:
	TSharedPtr<FUICommandList> DummyList;
};
