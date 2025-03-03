// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionTypes.h"

BEGIN_DEFINE_SPEC(FCommandListIdentifierSpec, "CommandsExtension.CommandListIdentifierSpec", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	FInputBindingManager* InputBindingManager;
END_DEFINE_SPEC(FCommandListIdentifierSpec)

void FCommandListIdentifierSpec::Define()
{
	Describe("FCommandListIdentifier tests ", [this]()
	{
		BeforeEach([this]()
		{
			InputBindingManager = &FInputBindingManager::Get();
			check(InputBindingManager != nullptr);
		});

		It("Identifier should not be valid", [this]()
		{
			const FCommandListIdentifier InvalidIdentifier = FCommandListIdentifier(NAME_None);
			TestFalse("Is identifier not valid", InvalidIdentifier.IsValid());
		});
		
		It("Identifier should be valid", [this]()
		{
			//TODO:: this assumes main frame registration, its a bad test!
			const FCommandListIdentifier ValidIdentifier = FCommandListIdentifier(TEXT("MainFrame"));
			TestTrue("Is identifier valid", ValidIdentifier.IsValid());
		});
		
		AfterEach([this]()
		{
			InputBindingManager = nullptr;
		});
	});
}