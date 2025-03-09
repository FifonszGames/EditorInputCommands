// Copyright FifonszGames. All Rights Reserved.


#include "ScopedCommand.h"

DEFINE_SPEC(FScopedCommandSpec, "CommandsExtension.ScopedCommandSpec", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)

void FScopedCommandSpec::Define()
{
	It("Should find valid command", [this]()
	{
		FInputBindingManager& BindingManager = FInputBindingManager::Get();
		const FScopedCommand ScopedCommand(BindingManager);
		const FInputCommandRegisterData& Data = ScopedCommand.GetData(); 
		const TSharedPtr<FUICommandInfo> CommandInfo = BindingManager.FindCommandInContext(Data.ContextProvider.GetBindingContextName(), Data.Identifier);
		TestTrue("Command is valid", CommandInfo.IsValid());	
	});
	
	It("Should not find command after it gets out of scope", [this]()
	{
		FInputBindingManager& BindingManager = FInputBindingManager::Get();
		FInputCommandRegisterData Data;
		
		{
			const FScopedCommand ScopedCommand(BindingManager);
			Data = ScopedCommand.GetData(); 
		}
			
		const TSharedPtr<FUICommandInfo> CommandInfo = BindingManager.FindCommandInContext(Data.ContextProvider.GetBindingContextName(), Data.Identifier);
		TestFalse("Command is valid", CommandInfo.IsValid());
	});
}