// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FCommandIdentifierPinFactory;

class FCommandsExtensionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	const TArray<TWeakPtr<FUICommandList>>* GetCommandListsForContext(const FName& ContextName) const;
	TArray<FName> GetAvailableContexts() const;

	void TryAddUnregisteredLists();
	
	static FName GetModuleName();
private:
	void OnRegisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList);
	void OnUnregisterCommandList(const FName ContextName, TSharedRef<FUICommandList> CommandList);

	TMap<FName, TArray<TWeakPtr<FUICommandList>>> CommandLists;
	TSharedPtr<FCommandIdentifierPinFactory> IdentifierPinFactory;
};
