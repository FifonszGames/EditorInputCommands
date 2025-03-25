// Copyright FifonszGames 2025 All Rights Reserved.


#include "ScopedCommand.h"
#include "Framework/Commands/InputBindingManager.h"
#include "Styling/AppStyle.h"

uint16 FScopedCommand::ScopedCommandCount = 0;
	
FScopedCommand::FScopedCommand(FInputBindingManager& InBindingManager, const FInputChord& DefaultChord) : BindingManager(&InBindingManager)
{
	++ScopedCommandCount;
	const FName ContextName(FString::Printf(TEXT("ScopedContext_%d"), ScopedCommandCount));
	BindingContext = MakeShared<FBindingContext>(ContextName,
		FText::FromString(FString::Printf(TEXT("%s_Desc"), *ContextName.ToString())),
		NAME_None,
		FAppStyle::GetAppStyleSetName());
		

	//ideally we would use binding manager (to create command info) passed to the constructor but members are private
	const FName CommandName(FString::Printf(TEXT("ScopedCommand_%d"), ScopedCommandCount));
	FUICommandInfo::MakeCommandInfo(BindingContext.ToSharedRef(), CommandInfo,
		CommandName,
		FText::FromName(CommandName),
		FText::FromString(FString::Printf(TEXT("%s_Desc"), *CommandName.ToString())),
		FInputCommandRegisterData::GetIcon(),
		EUserInterfaceActionType::None,
		DefaultChord);

	RegisterData = FInputCommandRegisterData(BindingContext.ToSharedRef(), CommandInfo.ToSharedRef());
}

FScopedCommand::FScopedCommand(const FInputChord& DefaultChord) : FScopedCommand(FInputBindingManager::Get(), DefaultChord)
{
}

FScopedCommand::~FScopedCommand()
{
	if (BindingManager)
	{
		BindingManager->RemoveInputCommand(BindingContext.ToSharedRef(), CommandInfo.ToSharedRef());
		BindingManager->RemoveContextByName(BindingContext->GetContextName());
	}
	
	BindingContext.Reset();
	CommandInfo.Reset();
	ScopedCommandCount--;
}

FName FScopedCommand::GetBindingContextName() const
{
	return RegisterData.ContextProvider.GetBindingContextName();
}

FScopedCommandWithList::FScopedCommandWithList(FInputBindingManager& InBindingManager, const FInputChord& DefaultChord) : FScopedCommand(InBindingManager, DefaultChord),
	DummyList(InBindingManager.RegisterNewCommandList(GetBindingContextName()))
{
}

FScopedCommandWithList::FScopedCommandWithList(const FInputChord& DefaultChord) : FScopedCommand(DefaultChord),
	DummyList(BindingManager->RegisterNewCommandList(GetBindingContextName()))
{
}

FScopedCommandWithList::~FScopedCommandWithList()
{
	if (DummyList.IsValid())
	{
		if (BindingManager)
		{
			const bool bDummyBool = BindingManager->UnregisterCommandList(GetBindingContextName(), DummyList.ToSharedRef());
		}
		DummyList.Reset();
	}
}
