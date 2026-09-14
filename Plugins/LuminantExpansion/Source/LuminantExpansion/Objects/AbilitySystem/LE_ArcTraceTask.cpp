// Copyright 2025, Luminant. All Rights Reserved.

#include "LE_ArcTraceTask.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "LE_AbilityObject.h"
#include "LE_AbilityComponent.h"

ULE_ArcTraceTask* ULE_ArcTraceTask::CreateArcTraceTask(
    ULE_AbilityObject* AbilityObject,
    USceneComponent* VectorComponent,
    FVector PositionOffset,
    FRotator RotationOffset,
    FVector LaunchVelocity,
    float SimulationDistance,
    float ProjectileRadius,
    float OverrideGravityZ,
    EArcTraceMode Mode,
    float Interval,
    ECollisionChannel TraceChannel,
    bool bDrawDebug
)
{
    if (!AbilityObject || !VectorComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create ArcTraceTask: Invalid parameters!"));
        return nullptr;
    }

    ULE_ArcTraceTask* NewTask = NewObject<ULE_ArcTraceTask>(AbilityObject);
    if (NewTask)
    {
        NewTask->SetTracingActor_Server(AbilityObject);
        NewTask->VectorComponent = VectorComponent;
        NewTask->PositionOffset = PositionOffset;
        NewTask->RotationOffset = RotationOffset;
        NewTask->LaunchVelocity = LaunchVelocity;
        NewTask->SimulationDistance = SimulationDistance;
        NewTask->ProjectileRadius = ProjectileRadius;
        NewTask->OverrideGravityZ = OverrideGravityZ;
        NewTask->Mode = Mode;
        NewTask->Interval = (Mode == EArcTraceMode::Continuous) ? FMath::Max(0.01f, Interval) : 0.0f;
        NewTask->Channel = TraceChannel;
        NewTask->bIsRunning = false;
        NewTask->bDrawDebug = bDrawDebug;
    }

    NewTask->OwningAbility = NewTask->GetTypedOuter<ULE_AbilityObject>();
    if (NewTask->OwningAbility)
    {
        NewTask->OwningAbility->AbilityTasks.Add(NewTask);
        NewTask->OwningAbility->StopArcTrace.AddDynamic(NewTask, &ULE_ArcTraceTask::StopTrace);
    }
    else
    {
        NewTask->ConditionalBeginDestroy();
        NewTask = nullptr;
    }

    return NewTask;
}

void ULE_ArcTraceTask::Activate()
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

void ULE_ArcTraceTask::StopTrace()
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

void ULE_ArcTraceTask::PerformTrace()
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

    // Build arc params
    FPredictProjectilePathParams Params;
    Params.StartLocation = Start;
    Params.LaunchVelocity = CombinedRotation.RotateVector(LaunchVelocity) * 1000;
    Params.ProjectileRadius = ProjectileRadius;
    Params.MaxSimTime = SimulationDistance;
    Params.SimFrequency = 20;
    Params.OverrideGravityZ = OverrideGravityZ;
    Params.TraceChannel = Channel;
    Params.bTraceWithCollision = true;
    Params.ActorsToIgnore.Add(TracingActor);

    FPredictProjectilePathResult Result;
    bool bHit = UGameplayStatics::PredictProjectilePath(World, Params, Result);

    if (bDrawDebug)
    {
        // Draw the arc
        for (int32 i = 0; i < Result.PathData.Num() - 1; i++)
        {
            DrawDebugLine(World, Result.PathData[i].Location, Result.PathData[i + 1].Location, FColor::Yellow, false, Interval);
        }
        if (bHit)
        {
            DrawDebugPoint(World, Result.HitResult.ImpactPoint, 10.0f, FColor::Blue, false, Interval);
        }
    }

    if (bHit)
    {
        OnHit.Broadcast(Result.HitResult);
    }
    else
    {
        OnMiss.Broadcast(Result.HitResult);
    }

    // Mode handling
    if (Mode == EArcTraceMode::Single)
    {
        StopTrace();
    }
    else if (Mode == EArcTraceMode::Continuous)
    {
        ScheduleNextTrace();
    }
}

void ULE_ArcTraceTask::ScheduleNextTrace()
{
    if (!bIsRunning && Interval < 0.001f) return;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TraceTimerHandle,
            this,
            &ULE_ArcTraceTask::PerformTrace,
            Interval,
            false
        );
    }
}

void ULE_ArcTraceTask::StopTask()
{
    if (OwningAbility)
    {
        OwningAbility->StopArcTrace.RemoveDynamic(this, &ULE_ArcTraceTask::StopTrace);
    }
    OwningAbility = nullptr;
    SetReadyToDestroy();
}

void ULE_ArcTraceTask::SetTracingActor_Server_Implementation(ULE_AbilityObject* StartObject)
{
    if (StartObject && StartObject->AbilityComponent)
    {
        if (StartObject->AbilityComponent->GetOwner())
        {
            TracingActor = StartObject->AbilityComponent->GetOwner();
            SetTracingActor_Client(TracingActor);
        }
    }
}

bool ULE_ArcTraceTask::SetTracingActor_Server_Validate(ULE_AbilityObject* StartObject)
{
    return true;
}

void ULE_ArcTraceTask::SetTracingActor_Client_Implementation(AActor* NewOwner)
{
    TracingActor = NewOwner;
}
