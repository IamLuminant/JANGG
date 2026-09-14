// Copyright 2025, Luminant. All Rights Reserved.


#include "SingleInputActivation.h"
#include "EnhancedInputComponent.h"

void USingleInputActivation::InitializeActivation_Implementation(UEnhancedInputComponent* InputComponent)
{
	Super::InitializeActivation_Implementation(InputComponent); // caches EnhancedInputComponent per your earlier setup

	if (!InputAction) // <- whatever your actual UInputAction* property is named
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no InputAction assigned - skipping bind."), *GetName());
		return;
	}

	if (!InputComponent)
	{
		return;
	}

	// Bind the action directly to this UObject's internal proxy function
	EnhancedInputComponent->BindAction(InputAction, TriggerEvent, this, &USingleInputActivation::HandleInputExecution);
}

void USingleInputActivation::HandleInputExecution()
{
	// Broadcast the blueprint-assignable event to whatever system is listening 
	// (Ability System, Interaction System, etc.)
	if (OnActivationEvent.IsBound())
	{
		OnActivationEvent.Broadcast();
	}
}

