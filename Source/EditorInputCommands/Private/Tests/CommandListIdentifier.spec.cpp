﻿// Copyright FifonszGames 2025 All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "EditorInputCommandTypes.h"
#include "Templates/SharedPointer.h"
#include "Framework/Commands/UICommandList.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FCommandListIdentifierSpec, "EditorInputCommands.CommandListIdentifier", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	TSharedPtr<FScopedCommandWithList> ScopedCommand;
END_DEFINE_SPEC(FCommandListIdentifierSpec)

void FCommandListIdentifierSpec::Define()
{
	BeforeEach([this]()
	{
		ScopedCommand = MakeShared<FScopedCommandWithList>();
	});
	
	It("Should not be valid", [this]()
	{
		const FCommandListIdentifier InvalidIdentifier;
		TestFalse("Is identifier not valid", InvalidIdentifier.IsValid());
	});
	
	It("Should have valid command list", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand->GetBindingContextName());
		TestTrue("Has valid command list", ValidIdentifier.HasValidCommandList());
	});
		
	It("Should be valid", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand->GetBindingContextName());
		TestTrue("Identifier is valid", ValidIdentifier.IsValid());
	});
	
	It("Foreach should return false on invalid identifier", [this]()
	{
		const FCommandListIdentifier InvalidIdentifier;
		const bool bForeachResult = InvalidIdentifier.ForeachCommandList([](const TSharedRef<FUICommandList>& List){ return true; });
		TestFalse("Result is false", bForeachResult);
	});
	
	It("Should have at least one command related list", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand->GetBindingContextName());
		const TArray<TWeakPtr<FUICommandList>>* Lists = ValidIdentifier.GetRelatedCommandLists();
		TestTrue("Result is true", Lists && !Lists->IsEmpty());
	});

	It("Foreach should return true on valid identifier", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand->GetBindingContextName());
		const bool bForeachResult = ValidIdentifier.ForeachCommandList([](const TSharedRef<FUICommandList>& List){ return true; });
		TestTrue("Result is true", bForeachResult);
	});
		
	AfterEach([this]()
	{
		ScopedCommand.Reset();
	});
}