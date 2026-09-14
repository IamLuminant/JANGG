// Copyright 2025, Luminant. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LE_AbilityTask.h"
#include "TimerManager.h"
#include "LE_WaitTimer.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTimerFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerFinishedBP);
DECLARE_MULTICAST_DELEGATE(FBroadcastTimerFinished);

class ULE_AbilityObject;

UCLASS()
class LUMINANTEXPANSION_API ULE_WaitTimer : public ULE_AbilityTask
{
	GENERATED_BODY()

public:

	ULE_WaitTimer();
	UFUNCTION(BlueprintCallable, Category = "LuminantExpansion", meta = (DefaultToSelf = "AbilityObject", AdvancedDisplay = "AbilityObject", BlueprintInternalUseOnly = "TRUE"))
	static ULE_WaitTimer* WaitTime(float TimeToWait, UPARAM(meta = (NotAllowNone)) ULE_AbilityObject* AbilityObject);
	virtual void Activate() override;

	virtual void StopTask() override;

	FOnTimerFinished OnTimerFinishedCPP;
	UPROPERTY(BlueprintAssignable)
	FOnTimerFinishedBP OnTimerFinished;

	FBroadcastTimerFinished BroadcastTimerFinishedHandle;

private:

	FTimerHandle TimerHandle;
	float TimeToWait;

	void TimerFinished();

	void BroadcastTimerFinished();


	
};
