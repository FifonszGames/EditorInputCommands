// Copyright FifonszGames. All Rights Reserved.


#include "EditorInputCommandTypes.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FExistingContentBindingSpec, "EditorInputCommands.ExistingContentBinding", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	TSharedPtr<FScopedCommand> ScopedCommand;
END_DEFINE_SPEC(FExistingContentBindingSpec)

void FExistingContentBindingSpec::Define()
{
	BeforeEach([this]()
	{
		ScopedCommand = MakeShared<FScopedCommand>();
	});
	
	It("Empty binding should not be valid", [this]()
	{
		const FExistingContextBinding InvalidBinding;
		TestFalse("Binding is valid", InvalidBinding.IsValid());
	});
	
	It("Should return valid context", [this]()
	{
		const FExistingContextBinding ValidBinding(ScopedCommand->GetBindingContextName());
		const TSharedPtr<FBindingContext> Context = ValidBinding.AsContext();
		TestTrue("Binding is valid", Context.IsValid());
	});

	It("Should point to the same binding context", [this]()
	{
		const FExistingContextBinding ValidBinding(ScopedCommand->GetBindingContextName());
		
		const TSharedPtr<FBindingContext> ScopedContext = ScopedCommand->GetData().GetContext();
		const TSharedPtr<FBindingContext> Binding = ValidBinding.AsContext();
		
		TestEqual("Bindings are the same", ScopedContext, Binding);
	});

	It("Should be valid", [this]()
	{
		const FExistingContextBinding ValidBinding(ScopedCommand->GetBindingContextName());
		TestTrue("Binding is valid", ValidBinding.IsValid());
	});
	
	AfterEach([this]()
	{
		ScopedCommand.Reset();
	});
}