// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionTypes.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FCommandListIdentifierSpec, "CommandsExtension.CommandListIdentifier", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	FInputBindingManager* InputBindingManager;
	FScopedCommand ScopedCommand;
	TSharedPtr<FUICommandList> DummyList;
END_DEFINE_SPEC(FCommandListIdentifierSpec)

void FCommandListIdentifierSpec::Define()
{
	BeforeEach([this]()
	{
		InputBindingManager = &FInputBindingManager::Get();
		check(InputBindingManager != nullptr);
		DummyList = InputBindingManager->RegisterNewCommandList(ScopedCommand.GetBindingContextName());
	});
	
	It("Should not be valid", [this]()
	{
		const FCommandListIdentifier InvalidIdentifier;
		TestFalse("Is identifier not valid", InvalidIdentifier.IsValid());
	});
	
	It("Should have valid command list", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand.GetBindingContextName());
		TestTrue("Has valid command list", ValidIdentifier.HasValidCommandList());
	});
		
	It("Should be valid", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand.GetBindingContextName());
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
		const FCommandListIdentifier ValidIdentifier(ScopedCommand.GetBindingContextName());
		const TArray<TWeakPtr<FUICommandList>>* Lists = ValidIdentifier.GetRelatedCommandLists();
		TestTrue("Result is true", Lists && !Lists->IsEmpty());
	});

	It("Foreach should return true on valid identifier", [this]()
	{
		const FCommandListIdentifier ValidIdentifier(ScopedCommand.GetBindingContextName());
		const bool bForeachResult = ValidIdentifier.ForeachCommandList([](const TSharedRef<FUICommandList>& List){ return true; });
		TestTrue("Result is true", bForeachResult);
	});
		
	AfterEach([this]()
	{
		if (DummyList.IsValid())
		{
			const bool bDummyBool = InputBindingManager->UnregisterCommandList(ScopedCommand.GetBindingContextName(), DummyList.ToSharedRef());
			DummyList.Reset();
		}
		InputBindingManager = nullptr;
	});
}