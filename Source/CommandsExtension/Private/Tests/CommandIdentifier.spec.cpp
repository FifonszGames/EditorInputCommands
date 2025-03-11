// Copyright FifonszGames. All Rights Reserved.


#include "CommandExtensionTypes.h"
#include "ScopedCommand.h"

BEGIN_DEFINE_SPEC(FCommandIdentifierSpec, "CommandsExtension.CommandIdentifier", EAutomationTestFlags::EngineFilter | EAutomationTestFlags_ApplicationContextMask)
	TSharedPtr<FScopedCommand> ScopedCommand;
	TSharedPtr<FInputChord> Chord;
END_DEFINE_SPEC(FCommandIdentifierSpec)

void FCommandIdentifierSpec::Define()
{
	BeforeEach([this]()
	{
		ScopedCommand = MakeShared<FScopedCommand>();
	});
	
	It("Should not be able to point to valid command", [this]()
	{
		const FCommandIdentifier InvalidData;
		TestFalse("Data is not valid", InvalidData.CanEverPointToValidCommand());
	});

	It("Should be able to point to valid command", [this]()
	{
		const FCommandIdentifier ValidData(ScopedCommand->GetData());
		TestTrue("Data is not valid", ValidData.CanEverPointToValidCommand());
	});
	
	It("Should return invalid info", [this]()
	{
		const FCommandIdentifier InvalidData;
		const bool bResult = InvalidData.AsInfo().IsValid();
		TestFalse("Command info is valid", bResult);
	});
	
	It("Should return valid info", [this]()
	{
		const FCommandIdentifier ValidData(ScopedCommand->GetData());
		const bool bResult = ValidData.AsInfo().IsValid();
		TestTrue("Command info is valid", bResult);
	});

	It("Should return invalid context", [this]()
	{
		const FCommandIdentifier InvalidData;
		const bool bResult = InvalidData.AsContext().IsValid();
		TestFalse("Command context is valid", bResult);
	});
	
	It("Should return valid context", [this]()
	{
		const FCommandIdentifier ValidData(ScopedCommand->GetData());
		const bool bResult = ValidData.AsContext().IsValid();
		TestTrue("Command context is valid", bResult);
	});

	It("Should not be registered", [this]()
	{
		const FCommandIdentifier InvalidData;
		const bool bResult = InvalidData.IsRegistered();
		TestFalse("Is registered", bResult);
	});
	
	It("Should be registered", [this]()
	{
		const FCommandIdentifier ValidData(ScopedCommand->GetData());
		const bool bResult = ValidData.IsRegistered();
		TestTrue("Is registered", bResult);
	});

	Describe("Chord tests", [this]()
	{
		BeforeEach([this]()
		{
			Chord = MakeShared<FInputChord>(EKeys::Apostrophe, true, true, true, true);
			ScopedCommand->GetCommandInfo()->SetActiveChord(*Chord, EMultipleKeyBindingIndex::Primary);
		});
		
		It("Should not be able to return chord", [this]()
		{
			const FCommandIdentifier InvalidData;
			FInputChord OutChord;
			const bool bResult = InvalidData.GetUserDefinedChord(EMultipleKeyBindingIndex::Primary, OutChord);
			TestFalse("Getting user defined chord result", bResult);
		});
		
		It("Should return valid chord", [this]()
		{
			const FCommandIdentifier ValidData(ScopedCommand->GetData());
			FInputChord OutChord;
			const bool bResult = ValidData.GetUserDefinedChord(EMultipleKeyBindingIndex::Primary, OutChord);
			TestTrue("Getting user defined chord result", bResult);
			TestEqual("Command chords", OutChord, *Chord);
		});
		
		It("Should return different chord for secondary index", [this]()
		{
			const FCommandIdentifier ValidData(ScopedCommand->GetData());
			FInputChord OutChord;
			const bool bResult = ValidData.GetUserDefinedChord(EMultipleKeyBindingIndex::Secondary, OutChord);
			TestTrue("Getting user defined chord result", bResult);
			TestNotEqual("Command chords", OutChord, *Chord);
		});
		
		AfterEach([this]()
		{
			Chord.Reset();
		});
	});
	
	AfterEach([this]()
	{
		ScopedCommand.Reset();
	});
}