// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FCommandIdentifierPinFactory;

class FCommandsExtensionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TWeakPtr<FUICommandList> GetCommandListForContext(const FName& ContextName) const;
	TArray<FName> GetAvailableContexts() const;

	static FName GetModuleName();
	
private:
	void OnRegisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList);
	void OnUnregisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList);

	TMap<FName, TWeakPtr<FUICommandList>> CommandLists;
	TSharedPtr<FCommandIdentifierPinFactory> IdentifierPinFactory;
};
