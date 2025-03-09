// Copyright FifonszGames. All Rights Reserved.


#include "CommandsExtensionLibrary.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FCommandsExtensionLibrarySpec, "CommandsExtension.CommandsExtensionLibrary", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
END_DEFINE_SPEC(FCommandsExtensionLibrarySpec)

#define FUNCTION_TESTS_DESC(ClassName, Function) FString::Printf(TEXT("Function tests -> %s::%s"), *(ClassName::StaticClass()->GetName()), GET_FUNCTION_NAME_STRING_CHECKED(ClassName, Function))

void FCommandsExtensionLibrarySpec::Define()
{
	Describe(FUNCTION_TESTS_DESC(UCommandsExtensionLibrary, RegisterInputCommand), [this]()
	{
		auto GetFunctionResultString = [](ERegistrationResult Result)
		{
			return FString::Printf(TEXT("Should return %s"), *UEnum::GetDisplayValueAsText(Result).ToString());
		};
		
		auto GetEqualTestString = [](ERegistrationResult Result)
		{
			return FString::Printf(TEXT("Result is %s"), *UEnum::GetDisplayValueAsText(Result).ToString());
		};
		
		It(GetFunctionResultString(ERegistrationResult::InvalidRegistrationData), [this, GetEqualTestString]()
		{
			const FInputCommandRegisterData InvalidData;
			ERegistrationResult Result = ERegistrationResult::Success;
			UCommandsExtensionLibrary::RegisterInputCommand(InvalidData, Result);
			TestEqual(GetEqualTestString(ERegistrationResult::InvalidRegistrationData), Result, ERegistrationResult::InvalidRegistrationData);
		});
		
		It(GetFunctionResultString(ERegistrationResult::AlreadyRegistered), [this, GetEqualTestString]()
		{
			const FScopedCommand Command;
			const FInputCommandRegisterData& SameData = Command.GetData();
			
			ERegistrationResult Result = ERegistrationResult::Success;
			UCommandsExtensionLibrary::RegisterInputCommand(SameData, Result);
			TestEqual(GetEqualTestString(ERegistrationResult::AlreadyRegistered), Result, ERegistrationResult::AlreadyRegistered);
		});

		It(GetFunctionResultString(ERegistrationResult::Success), [this, GetEqualTestString]()
		{
			const FScopedCommand ScopedCommand;
			
			FInputCommandRegisterData ValidData;
			{
				ValidData.ContextProvider = FExistingContextBinding(ScopedCommand.GetBindingContextName());
				ValidData.Identifier = FName(FGuid::NewGuid().ToString());
				ValidData.Label = FText::FromString(FGuid::NewGuid().ToString());
				ValidData.Description = FText::FromString(FGuid::NewGuid().ToString());
			}
			
			ERegistrationResult Result = ERegistrationResult::InvalidRegistrationData;
			UCommandsExtensionLibrary::RegisterInputCommand(ValidData, Result);
			TestEqual(GetEqualTestString(ERegistrationResult::Success), Result, ERegistrationResult::Success);
			
			//not a big fan of that :/
			UCommandsExtensionLibrary::UnregisterInputCommand(ValidData.GetIdentifier());
		});
	});

	Describe(FUNCTION_TESTS_DESC(UCommandsExtensionLibrary, UnregisterInputCommand), [this]()
	{
		It("Should not have to unregister invalid data", [this]()
		{
			const FInputCommandRegisterData InvalidData;
			const bool bResult = UCommandsExtensionLibrary::UnregisterInputCommand(InvalidData.GetIdentifier());
			TestFalse("Is result false", bResult);
		});
		
		It("Should properly unregister command", [this]()
		{
			const FScopedCommand ScopedCommand;
			const FInputCommandRegisterData& ValidData = ScopedCommand.GetData();
			
			const bool bResult = UCommandsExtensionLibrary::UnregisterInputCommand(ValidData.GetIdentifier());
			TestTrue("Is result true", bResult);
		});
	});
	
	// It("GetBindingContextNames() should not return an empty array", [this]()
	// {
	// 	const TArray<FName> Names = UCommandsExtensionLibrary::GetBindingContextNames();
	// 	TestFalse("Binding names are empty", Names.IsEmpty());
	// });
	//
	// It("GetCommandNames() should not return an empty array", [this]()
	// {
	// 	const TArray<FName> Names = UCommandsExtensionLibrary::GetCommandNames();
	// 	TestFalse("Command names are empty", Names.IsEmpty());
	// });
	//
	// It("GetCommandListIdentifiers() should not return an empty array", [this]()
	// {
	// 	const TArray<FName> Names = UCommandsExtensionLibrary::GetCommandNames();
	// 	TestFalse("Command list identifier names are empty", Names.IsEmpty());
	// });
	//
	//
	// Describe(FUNCTION_TESTS_DESC(UCommandsExtensionLibrary, IsCommandRegistered), [this]()
	// {
	// 	It("Should return false for invalid identifier", [this]()
	// 	{
	// 		const FCommandIdentifier InvalidIdentifier = FCommandIdentifier(NAME_None, NAME_None);
	// 		const bool bResult = UCommandsExtensionLibrary::IsCommandRegistered(InvalidIdentifier);
	// 		TestFalse("Identifier is not registered", bResult);
	// 	});
	// 	
	// 	It("Should return true for valid identifier", [this]()
	// 	{
	// 		const FCommandIdentifier ValidIdentifier = FCommandIdentifier(NAME_None, NAME_None);
	// 		const bool bResult = UCommandsExtensionLibrary::IsCommandRegistered(ValidIdentifier);
	// 		TestTrue("Identifier is registered", bResult);
	// 	});
	// });
}