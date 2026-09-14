// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActivationDataObject.h"
#include "EnhancedInputComponent.h"
#include "SingleInputActivation.generated.h"

class UInputAction;

UCLASS()
class LUMINANTEXPANSION_API USingleInputActivation : public UActivationDataObject
{
	GENERATED_BODY()

public:
	virtual void InitializeActivation_Implementation(UEnhancedInputComponent* InputComponent) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InputAction; // Renamed from MoveAction for generic capability

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	ETriggerEvent TriggerEvent = ETriggerEvent::Triggered;

	// Proxy function that catches the input and routes it to the delegate
	void HandleInputExecution();
	
};
