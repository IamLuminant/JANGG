// Copyright 2025, Luminant. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "LE_AbilityTask.h"
#include "InputTriggers.h"
#include "LEAT_WaitInput.generated.h"

class UInputAction;
class UEnhancedInputComponent;
struct FInputActionInstance;

/** MatchedAction: which UInputAction fired. MatchedTrigger: the ETriggerEvent it fired with. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputMatched, ULEAT_WaitInput*, Task);


/**
 * Waits on a set of InputActions, each flagged for either press or release,
 * and fires once whichever comes first, then ends.
 * 
 * Input binding happens on the local machine (client owning the ability).
 * The callback event fires on the machine that called WaitForInputs (server or client).
 */
UCLASS(meta = (ToolTip =
	"Broadcasts OnInputMatched whenever a watched input fires. Only ETriggerEvent::Completed auto-stops this task — for Started/Ongoing/Triggered/Canceled you must call Stop Task yourself when done, or it waits forever."))

class LUMINANTEXPANSION_API ULEAT_WaitInput : public ULE_AbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (ToolTip =
		"Fires on every match. This task does NOT stop itself unless the trigger was Completed — call Stop Task manually otherwise."))
	FInputMatched OnInputMatched;

	/** InputsToWatch: value is the specific ETriggerEvent to bind and wait for on that action. */
	UFUNCTION(BlueprintCallable, Category = "Luminant|Ability|Tasks",
		meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "OuterAbility"))
	static ULEAT_WaitInput* WaitForInputs(ULE_AbilityObject* OuterAbility, const TMap<UInputAction*, ETriggerEvent>& InputsToWatch);

	virtual void Activate() override;
	virtual void StopTask() override;

protected:
	UPROPERTY()
	TMap<TObjectPtr<UInputAction>, ETriggerEvent> WatchedInputs;

	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> BoundInputComponent;

	TArray<uint32> BindingHandles;

	UFUNCTION()
	void HandleInputTriggered(const FInputActionInstance& Instance);

	void BindAll();
	void UnbindAll();
};