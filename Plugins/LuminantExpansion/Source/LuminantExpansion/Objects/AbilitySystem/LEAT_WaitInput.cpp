// Copyright 2025, Luminant. All Rights Reserved.
#include "LEAT_WaitInput.h"
#include "LE_AbilityObject.h"
#include "LE_AbilityComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

ULEAT_WaitInput* ULEAT_WaitInput::WaitForInputs(ULE_AbilityObject* OwningAbility, const TMap<UInputAction*, ETriggerEvent>& InputsToWatch)
{
	ULEAT_WaitInput* Task = NewObject<ULEAT_WaitInput>();
	Task->OwningAbility = OwningAbility;

	for (const TPair<UInputAction*, ETriggerEvent>& Pair : InputsToWatch)
	{
		if (Pair.Key)
		{
			Task->WatchedInputs.Add(Pair.Key, Pair.Value);
		}
	}

	if (OwningAbility)
	{
		OwningAbility->AbilityTasks.Add(Task);
	}

	return Task;
}

void ULEAT_WaitInput::Activate()
{
	if (!OwningAbility || !OwningAbility->AbilityComponent)
	{
		StopTask();
		return;
	}

	// Only bind on the owning client/server that created this task
	// Input binding is client-side only, so this will only execute on the machine
	// that owns the EnhancedInputComponent
	BoundInputComponent = OwningAbility->AbilityComponent->EnhancedInputComponent;
	if (!BoundInputComponent)
	{
		StopTask();
		return;
	}

	BindAll();
}

void ULEAT_WaitInput::BindAll()
{
	for (const TPair<TObjectPtr<UInputAction>, ETriggerEvent>& Pair : WatchedInputs)
	{
		UInputAction* Action = Pair.Key;
		if (!Action)
		{
			continue;
		}

		FEnhancedInputActionEventBinding& Binding = BoundInputComponent->BindAction(
			Action, Pair.Value, this, &ULEAT_WaitInput::HandleInputTriggered);
		BindingHandles.Add(Binding.GetHandle());
	}
}

void ULEAT_WaitInput::HandleInputTriggered(const FInputActionInstance& Instance)
{
	const UInputAction* FiredAction = Instance.GetSourceAction();
	const ETriggerEvent Trigger = Instance.GetTriggerEvent();

	for (const TPair<TObjectPtr<UInputAction>, ETriggerEvent>& Pair : WatchedInputs)
	{
		if (Pair.Key.Get() == FiredAction)
		{
			// Client called WaitForInputs, broadcast locally on client or player authority
			OnInputMatched.Broadcast(this);

			StopTask();
			break;
		}
	}
}

void ULEAT_WaitInput::UnbindAll()
{
	if (BoundInputComponent)
	{
		for (uint32 Handle : BindingHandles)
		{
			BoundInputComponent->RemoveBindingByHandle(Handle);
		}
	}
	BindingHandles.Reset();
}

void ULEAT_WaitInput::StopTask()
{
	UnbindAll();

	// Remove ourselves from the owning ability's task list so it doesn't hold
	// a stale pointer after we self-complete (rather than being stopped by
	// DeactivateAbility). Consider moving this into ULE_AbilityTask::StopTask
	// if every task subclass needs the same cleanup.
	if (OwningAbility)
	{
		OwningAbility->AbilityTasks.RemoveSingleSwap(this);
	}

	Super::StopTask();
	SetReadyToDestroy();
}