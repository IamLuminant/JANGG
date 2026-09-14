// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_AbilityTask.h"
#include "LE_AbilityObject.h"

void ULE_AbilityTask::StopTask()
{
	DelegateHandle.Reset();
	if (OwningAbility)
	{
		OwningAbility = nullptr;
	}
}

