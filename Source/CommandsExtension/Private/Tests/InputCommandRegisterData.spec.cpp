// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionTypes.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FInputCommandRegisterDataSpec, "CommandsExtension.InputCommandRegisterData", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	TSharedPtr<FScopedCommand> ScopedCommand;
END_DEFINE_SPEC(FInputCommandRegisterDataSpec)

void FInputCommandRegisterDataSpec::Define()
{
	BeforeEach([this]()
	{
		ScopedCommand = MakeShared<FScopedCommand>();
	});
	
	It("Should not be valid", [this]()
	{
		const FInputCommandRegisterData InvalidData = FInputCommandRegisterData();
		TestFalse("Data is not valid", InvalidData.IsValid());
	});
	
	It("Should be valid", [this]()
	{
		const FInputCommandRegisterData& ValidData = ScopedCommand->GetData();
		TestTrue("Data is not valid", ValidData.IsValid());
	});

	It("Should return valid context", [this]()
	{
		const FInputCommandRegisterData& ValidData = ScopedCommand->GetData();
		TSharedPtr<FBindingContext> Context = ValidData.GetContext();
		TestTrue("Context is valid", Context.IsValid());
	});

	It("Should return identifier with same context", [this]()
	{
		const FInputCommandRegisterData& ValidData = ScopedCommand->GetData();
		const FCommandIdentifier Identifier = ValidData.GetIdentifier();

		const TSharedPtr<FBindingContext> IdendifierContext = Identifier.AsContext();
		const TSharedPtr<FBindingContext> DataContext = ValidData.GetContext();
		TestEqual("Binding Contexts", IdendifierContext, DataContext);
	});
	
	AfterEach([this]()
	{
		ScopedCommand.Reset();
	});
}