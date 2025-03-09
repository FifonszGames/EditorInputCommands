// Copyright FifonszGames. All Rights Reserved.


#include "ScopedCommand.h"

DEFINE_SPEC(FScopedCommandSpec, "CommandsExtension.ScopedCommandSpec", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)

void FScopedCommandSpec::Define()
{
	It("Should return true for valid command and false after destructor is called", [this]()
	{
		FInputBindingManager& BindingManager = FInputBindingManager::Get();
		FInputCommandRegisterData Data;
		TSharedPtr<FUICommandInfo> CommandInfo;
		{
			FScopedCommand ScopedCommand(BindingManager);
			Data = ScopedCommand.GetData(); 
			CommandInfo = BindingManager.FindCommandInContext(Data.ContextProvider.GetBindingContextName(), Data.Identifier);
			TestTrue("Command is valid", CommandInfo.IsValid());	
		}
		
		CommandInfo = BindingManager.FindCommandInContext(Data.ContextProvider.GetBindingContextName(), Data.Identifier);
		TestFalse("Command is valid", CommandInfo.IsValid());
	});
}