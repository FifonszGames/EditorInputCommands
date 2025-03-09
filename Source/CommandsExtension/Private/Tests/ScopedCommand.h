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
	private:
		FInputCommandRegisterData RegisterData;
		TSharedPtr<FBindingContext> BindingContext;
		TSharedPtr<FUICommandInfo> CommandInfo;
	
		FInputBindingManager* BindingManager;

		static uint16 ScopedCommandCount;
};
