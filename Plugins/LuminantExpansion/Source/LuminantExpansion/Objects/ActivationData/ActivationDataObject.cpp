// Copyright 2025, Luminant. All Rights Reserved.


#include "ActivationDataObject.h"
#include "EnhancedInputComponent.h"

void UActivationDataObject::InitializeActivation_Implementation(UEnhancedInputComponent* InputComponent)
{
	if (InputComponent)
	{
		EnhancedInputComponent = InputComponent;
	}
}