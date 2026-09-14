// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActivationDataObject.generated.h"

class UEnhancedInputComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActivate);

UCLASS(Blueprintable, BlueprintType, Abstract, DefaultToInstanced, EditInlineNew)
class LUMINANTEXPANSION_API UActivationDataObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "LuminantsExpansions")
	UEnhancedInputComponent* EnhancedInputComponent;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, BlueprintReadWrite, Category = "LuminantsExpansions")
	FActivate OnActivationEvent;

	// Called by the Character/Controller to initialize input bindings
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LuminantsExpansions")
	void InitializeActivation(UEnhancedInputComponent* InputComponent);

	
};
