// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "EdGraphUtilities.h"

class FCommandIdentifierPinFactory: public FGraphPanelPinFactory
{
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};
