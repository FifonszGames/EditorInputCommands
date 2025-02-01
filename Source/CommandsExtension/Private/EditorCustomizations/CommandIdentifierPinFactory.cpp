// Copyright FifonszGames. All Rights Reserved.


#include "EditorCustomizations/CommandIdentifierPinFactory.h"

#include "CommandsExtensionLibrary.h"
#include "K2Node_CallFunction.h"
#include "EditorCustomizations/CommandIdentifierPin.h"

TSharedPtr<SGraphPin> FCommandIdentifierPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (UK2Node_CallFunction* OwningNode = Cast<UK2Node_CallFunction>(Pin->GetOwningNode()))
	{
		if (OwningNode->FunctionReference.GetMemberParentClass() == UCommandsExtensionLibrary::StaticClass())
		{
			if (Pin && Pin->Direction == EGPD_Input && Pin->PinName.IsEqual(TEXT("Identifier")))
			{
				return SNew(SCommandIdentifierPin, Pin);
			}
		}
	}
	return FGraphPanelPinFactory::CreatePin(Pin);
}
