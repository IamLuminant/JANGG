// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LE_AbilityTask.generated.h"

class ULE_AbilityObject;

UCLASS(Blueprintable)
class LUMINANTEXPANSION_API ULE_AbilityTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	FDelegateHandle DelegateHandle;

	virtual void StopTask();

protected:

	UPROPERTY()
	ULE_AbilityObject* OwningAbility;
	
};
