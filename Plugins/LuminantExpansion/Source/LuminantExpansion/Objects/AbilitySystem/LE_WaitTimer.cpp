// Copyright 2025, Luminant. All Rights Reserved.

#include "LE_WaitTimer.h"
#include "LE_AbilityObject.h"
#include "Engine/World.h"
#include "TimerManager.h"

ULE_WaitTimer::ULE_WaitTimer()
{
	TimeToWait = 0.0f;
}

ULE_WaitTimer* ULE_WaitTimer::WaitTime(float TimeToWait, ULE_AbilityObject* CallingObject)
{
	if (!CallingObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create WaitTimer: Invalid CallingObject!"));
		return nullptr;
	}
	ULE_WaitTimer* Node = NewObject<ULE_WaitTimer>(CallingObject);
	Node->TimeToWait = TimeToWait;
	ULE_AbilityObject* Ability = Cast<ULE_AbilityObject>(CallingObject);
	if (Ability)
	{
		Ability->AbilityTasks.Add(Node);
	}

	return Node;
}

void ULE_WaitTimer::Activate()
{
	Super::Activate();
	DelegateHandle = BroadcastTimerFinishedHandle.AddUObject(this, &ULE_WaitTimer::TimerFinished);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULE_WaitTimer::BroadcastTimerFinished, TimeToWait, false);
}

void ULE_WaitTimer::TimerFinished()
{
	OnTimerFinishedCPP.Broadcast();
	OnTimerFinished.Broadcast();
	RemoveFromRoot();
}

void ULE_WaitTimer::BroadcastTimerFinished()
{
	BroadcastTimerFinishedHandle.Broadcast();
}

void ULE_WaitTimer::StopTask()
{
	Super::StopTask();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}



