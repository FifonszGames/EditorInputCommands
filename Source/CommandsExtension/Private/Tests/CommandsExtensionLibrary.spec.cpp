// Copyright FifonszGames. All Rights Reserved.


#include "CommandsExtensionLibrary.h"
#include "LibrarySpecTestObject.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FCommandsExtensionLibrarySpec, "CommandsExtension.CommandsExtensionLibrary", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	TSharedPtr<FScopedCommandWithList> ScopedCommandWithList;
	TSharedPtr<FInputChord> Chord;
	TSharedPtr<FCommandMappingData> MappingData;
	TSharedPtr<FOnExecute> OnExecute;
	TObjectPtr<ULibrarySpecTestObject> TestObject;
	TSharedPtr<FModifierKeysState> KeyState;
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

	Describe("Mapping tests", [this]()
	{
		BeforeEach([this]()
		{
			Chord = MakeShared<FInputChord>(EKeys::Apostrophe, true, true, true, true);
			ScopedCommandWithList = MakeShared<FScopedCommandWithList>(*Chord);
			OnExecute = MakeShared<FOnExecute>();
						
			const FCommandListIdentifier ListIdentifier(ScopedCommandWithList->GetBindingContextName());
			const FCommandIdentifier CommandIdentifier(ScopedCommandWithList->GetData());
			MappingData = MakeShared<FCommandMappingData>(ListIdentifier, CommandIdentifier);
		});
		
		Describe(FUNCTION_TESTS_DESC(UCommandsExtensionLibrary, MapAction), [this]()
		{
			It("Should not map command to invalid data", [this]()
			{
				FCommandMappingData InvalidMappingData;
				const bool bResult = UCommandsExtensionLibrary::MapAction(InvalidMappingData, *OnExecute);
				TestFalse("Is result false", bResult);
			});
			
			It("Should properly map command", [this]()
			{
				const bool bResult = UCommandsExtensionLibrary::MapAction(*MappingData, *OnExecute);
				TestTrue("Is result ture", bResult);
			});
			
			Describe("Mapping with bindings", [this]()
			{
				BeforeEach([this]()
				{
					TestObject = NewObject<ULibrarySpecTestObject>();
					TestObject->bWasCalled = false;
					TestObject->bWasCalled2 = false;
					
					TestObject->AddToRoot();
					OnExecute->BindDynamic(TestObject, &ULibrarySpecTestObject::TestFunc);
					KeyState = MakeShareable(new FModifierKeysState(Chord->bShift, !Chord->bShift, Chord->bCtrl, !Chord->bCtrl, Chord->bAlt, !Chord->bAlt, Chord->bCmd, !Chord->bCmd, false));
				});
				
				It("Should properly execute event after receiving input", [this]()
				{
					const bool bWasActionMapped = UCommandsExtensionLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped", bWasActionMapped);
					
					const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
					TestTrue("Was command processed", bWasCommandProcessed);
					TestTrue("Was function called", TestObject->bWasCalled);
				});

				It("Should override event", [this]()
				{
					const bool bWasActionMapped = UCommandsExtensionLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped", bWasActionMapped);
					
					OnExecute->Unbind();
					OnExecute->BindDynamic(TestObject, &ULibrarySpecTestObject::TestFunc2);
					
					const bool bWasActionMapped2 = UCommandsExtensionLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped2", bWasActionMapped2);
					
					const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
					TestTrue("Was command processed", bWasCommandProcessed);
					TestFalse("Was function not called", TestObject->bWasCalled);
					TestTrue("Was function 2 called", TestObject->bWasCalled2);
				});

				It("Should not override action", [this]()
				{
					const bool bWasActionMapped = UCommandsExtensionLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped", bWasActionMapped);
					
					FOnExecute OnExecute2;
					OnExecute2.BindDynamic(TestObject, &ULibrarySpecTestObject::TestFunc2);

					const bool bWasActionMapped2 = UCommandsExtensionLibrary::MapAction(*MappingData, *OnExecute, false);
					TestFalse("Was action mapped2", bWasActionMapped2);
					
					const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
					TestTrue("Was command processed", bWasCommandProcessed);
					TestFalse("Was function 2 not called", TestObject->bWasCalled2);
					TestTrue("Was function called", TestObject->bWasCalled);
				});
				
				AfterEach([this]
				{
					if (TestObject)
					{
						TestObject->RemoveFromRoot();
					}
					KeyState.Reset();
				});
			});
		});
		
		// Describe(FUNCTION_TESTS_DESC(UCommandsExtensionLibrary, UnmapAction), [this]()
		// {
		// 	
		// });
		
		AfterEach([this]
		{
			ScopedCommandWithList.Reset();
			Chord.Reset();
			if (MappingData.IsValid())
			{
				UCommandsExtensionLibrary::UnmapAction(*MappingData);
				MappingData.Reset();
			}
			OnExecute.Reset();
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