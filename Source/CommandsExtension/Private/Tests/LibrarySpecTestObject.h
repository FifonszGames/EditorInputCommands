// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LibrarySpecTestObject.generated.h"

UCLASS(Transient)
class ULibrarySpecTestObject : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void TestFunc()
	{
		bWasCalled = true;
	}
	UFUNCTION()
	void TestFunc2()
	{
		bWasCalled2 = true;
	}
	bool bWasCalled = false;
	bool bWasCalled2 = false;
};