// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_LineTraceTask.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "LE_AbilityObject.h"
#include "LE_AbilityComponent.h"
#include "Engine/Engine.h"

ULE_LineTraceTask* ULE_LineTraceTask::CreateLineTraceTask(
    ULE_AbilityObject* AbilityObject,
	USceneComponent* VectorComponent,
	FVector PositionOffset,
	FRotator RotationOffset,
    float Length,
    ELineTraceMode Mode,
    float Interval,
    ECollisionChannel TraceChannel,
	bool bDrawDebug
    )
{
	if (!AbilityObject || !VectorComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create LineTraceTask: Invalid parameters!"));
		return nullptr;
	}
    ULE_LineTraceTask* NewTask = NewObject<ULE_LineTraceTask>(AbilityObject);
    if (NewTask)
    {
		NewTask->SetTracingActor_Server(AbilityObject);
		NewTask->VectorComponent = VectorComponent;
		NewTask->PositionOffset = PositionOffset;
		NewTask->RotationOffset = RotationOffset;
        NewTask->Length = Length;
        NewTask->Mode = Mode;
        NewTask->Interval = (Mode == ELineTraceMode::Continuous) ? FMath::Max(0.01f, Interval) : 0.0f;
        NewTask->Channel = TraceChannel;
        NewTask->bIsRunning = false;
		NewTask->bDrawDebug = bDrawDebug;
    }
    NewTask->OwningAbility = NewTask->GetTypedOuter<ULE_AbilityObject>();
    if (NewTask->OwningAbility)
    {
        NewTask->OwningAbility->AbilityTasks.Add(NewTask);
		NewTask->OwningAbility->StopLineTrace.AddDynamic(NewTask, &ULE_LineTraceTask::StopTrace);
	}
	else
	{
		NewTask->ConditionalBeginDestroy();
		NewTask = nullptr;
    }
    return NewTask;
}

void ULE_LineTraceTask::Activate()
{
    Super::Activate();

    if (!TracingActor)
    {
		UE_LOG(LogTemp, Warning, TEXT("TracingActor is null!"));
        return;
    }

    bIsRunning = true;
    PerformTrace();
}

void ULE_LineTraceTask::StopTrace()
{
    if (!bIsRunning) return;

    bIsRunning = false;

    if (TraceTimerHandle.IsValid())
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(TraceTimerHandle);
        }
    }

    OnTraceStopped.Broadcast();
	StopTask();
}

void ULE_LineTraceTask::PerformTrace()
{
    if (!bIsRunning || !TracingActor) return;

    UWorld* World = TracingActor->GetWorld();
    if (!World) return;

	FVector ComponentLocation = VectorComponent->GetComponentLocation();
	FQuat ComponentRotation = VectorComponent->GetComponentQuat();
	
	// Apply position offset
	FVector Start = ComponentLocation + PositionOffset;
	
	// Combine rotations: component rotation + offset rotation
	FQuat CombinedRotation = ComponentRotation * RotationOffset.Quaternion();
	FVector TraceDirection = CombinedRotation.GetForwardVector();
	FVector End = Start + (TraceDirection * Length);

    FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(TracingActor);
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        Channel,
		TraceParams
    );

    // Debug visualization
	if (bDrawDebug)
    {
        DrawDebugLine(World, Start, End, bHit ? FColor::Green : FColor::Red, false, Interval);
    }
    if (bHit)
    {
		if (bDrawDebug)
        {
            DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, FColor::Blue, false, Interval);
        }
		OnHit.Broadcast(HitResult);
    }
    else
    {
        OnMiss.Broadcast(HitResult);
    }

    // Handle mode-specific behavior
    if (Mode == ELineTraceMode::Single)
    {
        StopTrace();
    }
    else if (Mode == ELineTraceMode::Continuous)
    {
        ScheduleNextTrace();
    }
}

void ULE_LineTraceTask::ScheduleNextTrace()
{
    if (!bIsRunning && Interval <= 0.0f) return;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TraceTimerHandle,
            this,
            &ULE_LineTraceTask::PerformTrace,
            Interval,
            false
        );
    }
}

void ULE_LineTraceTask::StopTask()
{
	if (OwningAbility)
	{
		OwningAbility->StopLineTrace.RemoveDynamic(this, &ULE_LineTraceTask::StopTrace);
	}
	OwningAbility = nullptr;
    SetReadyToDestroy();
}


void ULE_LineTraceTask::SetTracingActor_Server_Implementation(ULE_AbilityObject* StartObject)
{
    if (StartObject)
    {
        if (StartObject->AbilityComponent)
        {
            if (StartObject->AbilityComponent->GetOwner())
            {
                TracingActor = StartObject->AbilityComponent->GetOwner();
				SetTracingActor_Client(TracingActor);
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create LineTraceTask: OwnerActor is null!"));
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create LineTraceTask: AbilityComponent is null!"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create LineTraceTask: TracingActor is null!"));
    }
}

bool ULE_LineTraceTask::SetTracingActor_Server_Validate(ULE_AbilityObject* StartObject)
{
	return true;
}

void ULE_LineTraceTask::SetTracingActor_Client_Implementation(AActor* NewOwner)
{
	TracingActor = NewOwner;
}

