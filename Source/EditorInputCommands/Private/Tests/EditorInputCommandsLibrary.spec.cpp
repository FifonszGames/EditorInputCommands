// Copyright FifonszGames. All Rights Reserved.


#include "EditorInputCommandsLibrary.h"
#include "LibrarySpecTestObject.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FEditorInputCommandsLibrarySpec, "EditorInputCommands.EditorInputCommandsLibrary", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	TSharedPtr<FScopedCommandWithList> ScopedCommandWithList;
	TSharedPtr<FInputChord> Chord;
	TSharedPtr<FModifierKeysState> KeyState;

	TSharedPtr<FCommandMappingData> MappingData;
	TSharedPtr<FOnExecute> OnExecute;
	TObjectPtr<ULibrarySpecTestObject> TestObject;
END_DEFINE_SPEC(FEditorInputCommandsLibrarySpec)

#define FUNCTION_TESTS_DESC(ClassName, Function) FString::Printf(TEXT("Function tests -> %s::%s"), *(ClassName::StaticClass()->GetName()), GET_FUNCTION_NAME_STRING_CHECKED(ClassName, Function))

void FEditorInputCommandsLibrarySpec::Define()
{
	BeforeEach([this]()
	{
		Chord = MakeShared<FInputChord>(EKeys::Apostrophe, true, true, true, true);
		KeyState = MakeShareable(new FModifierKeysState(Chord->bShift, !Chord->bShift, Chord->bCtrl, !Chord->bCtrl, Chord->bAlt, !Chord->bAlt, Chord->bCmd, !Chord->bCmd, false));
		ScopedCommandWithList = MakeShared<FScopedCommandWithList>(*Chord);
	});
	
	Describe(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, RegisterInputCommand), [this]()
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
			UEditorInputCommandsLibrary::RegisterInputCommand(InvalidData, Result);
			TestEqual(GetEqualTestString(ERegistrationResult::InvalidRegistrationData), Result, ERegistrationResult::InvalidRegistrationData);
		});
		
		It(GetFunctionResultString(ERegistrationResult::AlreadyRegistered), [this, GetEqualTestString]()
		{
			const FInputCommandRegisterData& SameData = ScopedCommandWithList->GetData();
			
			ERegistrationResult Result = ERegistrationResult::Success;
			UEditorInputCommandsLibrary::RegisterInputCommand(SameData, Result);
			TestEqual(GetEqualTestString(ERegistrationResult::AlreadyRegistered), Result, ERegistrationResult::AlreadyRegistered);
		});

		It(GetFunctionResultString(ERegistrationResult::Success), [this, GetEqualTestString]()
		{
			FInputCommandRegisterData ValidData;
			{
				ValidData.ContextProvider = FExistingContextBinding(ScopedCommandWithList->GetBindingContextName());
				ValidData.Identifier = FName(FGuid::NewGuid().ToString());
				ValidData.Label = FText::FromString(FGuid::NewGuid().ToString());
				ValidData.Description = FText::FromString(FGuid::NewGuid().ToString());
			}
			
			ERegistrationResult Result = ERegistrationResult::InvalidRegistrationData;
			UEditorInputCommandsLibrary::RegisterInputCommand(ValidData, Result);
			TestEqual(GetEqualTestString(ERegistrationResult::Success), Result, ERegistrationResult::Success);
			
			//not a big fan of that here :/
			UEditorInputCommandsLibrary::UnregisterInputCommand(ValidData.GetIdentifier());
		});
	});

	Describe(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, UnregisterInputCommand), [this]()
	{
		It("Should not have to unregister invalid data", [this]()
		{
			const FInputCommandRegisterData InvalidData;
			const bool bResult = UEditorInputCommandsLibrary::UnregisterInputCommand(InvalidData.GetIdentifier());
			TestFalse("Is result false", bResult);
		});
		
		It("Should properly unregister command", [this]()
		{
			const FScopedCommand ScopedCommand;
			const FInputCommandRegisterData& ValidData = ScopedCommand.GetData();
			
			const bool bResult = UEditorInputCommandsLibrary::UnregisterInputCommand(ValidData.GetIdentifier());
			TestTrue("Is result true", bResult);
		});
	});

	Describe("Mapping tests", [this]()
	{
		BeforeEach([this]()
		{
			TestObject = NewObject<ULibrarySpecTestObject>();
			TestObject->bWasCalled = false;
			TestObject->bWasCalled2 = false;
			TestObject->AddToRoot();
			
			OnExecute = MakeShared<FOnExecute>();
			OnExecute->BindDynamic(TestObject, &ULibrarySpecTestObject::TestFunc);
			
			const FCommandListIdentifier ListIdentifier(ScopedCommandWithList->GetBindingContextName());
			const FCommandIdentifier CommandIdentifier(ScopedCommandWithList->GetData());
			MappingData = MakeShared<FCommandMappingData>(ListIdentifier, CommandIdentifier);
		});
		
		Describe(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, MapAction), [this]()
		{
			It("Should not map command to invalid data", [this]()
			{
				FCommandMappingData InvalidMappingData;
				const bool bResult = UEditorInputCommandsLibrary::MapAction(InvalidMappingData, *OnExecute);
				TestFalse("Is result false", bResult);
			});
			
			It("Should properly map command", [this]()
			{
				const bool bResult = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute);
				TestTrue("Is result true", bResult);
			});
			
			Describe("Mapping with bindings", [this]()
			{
				It("Should properly execute event after receiving input", [this]()
				{
					const bool bWasActionMapped = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped", bWasActionMapped);
					
					const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
					TestTrue("Was command processed", bWasCommandProcessed);
					TestTrue("Was test function called", TestObject->bWasCalled);
				});

				It("Should override event", [this]()
				{
					const bool bWasActionMapped = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped", bWasActionMapped);
					
					OnExecute->Unbind();
					OnExecute->BindDynamic(TestObject, &ULibrarySpecTestObject::TestFunc2);
					
					const bool bWasActionMapped2 = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped2", bWasActionMapped2);
					
					const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
					TestTrue("Was command processed", bWasCommandProcessed);
					TestFalse("Was test function called", TestObject->bWasCalled);
					TestTrue("Was test function 2 called", TestObject->bWasCalled2);
				});

				It("Should not override action", [this]()
				{
					const bool bWasActionMapped = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute);
					TestTrue("Was action mapped", bWasActionMapped);
					
					FOnExecute OnExecute2;
					OnExecute2.BindDynamic(TestObject, &ULibrarySpecTestObject::TestFunc2);

					const bool bWasActionMapped2 = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute, false);
					TestFalse("Was action mapped2", bWasActionMapped2);
					
					const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
					TestTrue("Was command processed", bWasCommandProcessed);
					TestFalse("Was test function 2 called", TestObject->bWasCalled2);
					TestTrue("Was test function called", TestObject->bWasCalled);
				});
			});
		});
		
		Describe(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, UnmapAction), [this]()
		{
			It("Should not have to unmap action with invalid data", [this]()
			{
				const FCommandMappingData InvalidData;
				const bool bResult = UEditorInputCommandsLibrary::UnmapAction(InvalidData);
				TestFalse("Is result false", bResult);
			});
			
			It("Should have to unmap action", [this]()
			{
				const bool bWasActionMapped = UEditorInputCommandsLibrary::MapAction(*MappingData, *OnExecute);
				TestTrue("Was action mapped", bWasActionMapped);
				
				const bool bResult = UEditorInputCommandsLibrary::UnmapAction(*MappingData);
				TestFalse("Is result false", bResult);
				
				const bool bWasCommandProcessed = ScopedCommandWithList->GetList()->ProcessCommandBindings(Chord->Key, *KeyState, false);
				TestFalse("Was command processed", bWasCommandProcessed);
				TestFalse("Was test function called", TestObject->bWasCalled);
			});
		});
		
		AfterEach([this]
		{
			if (MappingData.IsValid())
			{
				UEditorInputCommandsLibrary::UnmapAction(*MappingData);
				MappingData.Reset();
			}
			OnExecute.Reset();
			
			if (TestObject)
			{
				TestObject->RemoveFromRoot();
			}
		});
	});
	
	It(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, GetBindingContextNames), [this]()
	{
		const TArray<FName> BindingContextNames = UEditorInputCommandsLibrary::GetBindingContextNames();
		const bool bResult = BindingContextNames.Contains(ScopedCommandWithList->GetBindingContextName());
		TestTrue("Binding names contain scoped command's binding context name", bResult);
	});
	
	It(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, GetCommandNames), [this]()
	{
		const TArray<FName> CommandNames = UEditorInputCommandsLibrary::GetCommandNames();
		const bool bResult = CommandNames.Contains(ScopedCommandWithList->GetData().Identifier);
		TestTrue("Command names contain scoped command's identifier", bResult);
	});
	
	It(FUNCTION_TESTS_DESC(UEditorInputCommandsLibrary, GetCommandListIdentifiers), [this]()
	{
		const TArray<FName> CommandListIdentifiers = UEditorInputCommandsLibrary::GetCommandListIdentifiers();
		const bool bResult = CommandListIdentifiers.Contains(ScopedCommandWithList->GetBindingContextName());
		TestTrue("Command list identifiers contain scoped command's list name", bResult);
	});
	
	AfterEach([this]()
	{
		ScopedCommandWithList.Reset();
		Chord.Reset();
		KeyState.Reset();
	});
}