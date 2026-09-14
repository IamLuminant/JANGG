// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LE_AbilityTask.h"
#include "Engine/EngineTypes.h"
#include "LE_ArcTraceTask.generated.h"

class ULE_AbilityObject;
class USceneComponent;

/** Arc trace mode, mirrors line trace task */
UENUM(BlueprintType)
enum class EArcTraceMode : uint8
{
    Single      UMETA(DisplayName = "Single"),
    Continuous  UMETA(DisplayName = "Continuous")
};

/** Delegate for arc trace results */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArcTraceHitSignature, const FHitResult&, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FArcTraceStoppedSignature);

/**
 * Async arc trace ability task
 */
UCLASS()
class LUMINANTEXPANSION_API ULE_ArcTraceTask : public ULE_AbilityTask
{
    GENERATED_BODY()

public:
    /** Factory to create an arc trace task */
    UFUNCTION(BlueprintCallable, Category = "Luminant|AbilityTasks", meta = (
        DefaultToSelf = "AbilityObject", 
        AdvancedDisplay = "AbilityObject, PositionOffset, RotationOffset, TraceChannel, ProjectileRadius, OverrideGravityZ", 
        BlueprintInternalUseOnly = "TRUE"))
    static ULE_ArcTraceTask* CreateArcTraceTask(
        UPARAM(meta = (NotAllowNone)) ULE_AbilityObject* AbilityObject,
        UPARAM(meta = (NotAllowNone)) USceneComponent* VectorComponent,
        FVector PositionOffset = FVector::ZeroVector,
        FRotator RotationOffset = FRotator::ZeroRotator,
        FVector LaunchVelocity = FVector::ZeroVector,
        float SimulationDistance = 1000.0f,
        float ProjectileRadius = 10.0f,
        float OverrideGravityZ = -980.0f,
        EArcTraceMode Mode = EArcTraceMode::Single,
        float Interval = 0.1f,
        ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility,
        bool bDrawDebug = false
    );

    /** Starts the task */
    virtual void Activate() override;

    /** Stops the trace */
    UFUNCTION()
    void StopTrace();

    /** Broadcast when a hit is found */
    UPROPERTY(BlueprintAssignable)
    FArcTraceHitSignature OnHit;

    /** Broadcast when a miss occurs */
    UPROPERTY(BlueprintAssignable)
    FArcTraceHitSignature OnMiss;

    /** Broadcast when the trace is stopped */
    UPROPERTY(BlueprintAssignable)
    FArcTraceStoppedSignature OnTraceStopped;

protected:
    /** Perform a trace */
    void PerformTrace();

    /** Schedule the next trace for continuous mode */
    void ScheduleNextTrace();

    /** Internal stop */
    virtual void StopTask() override;

    /** Set the tracing actor on server */
    UFUNCTION(Server, Reliable, WithValidation)
    void SetTracingActor_Server(ULE_AbilityObject* StartObject);

    /** Set the tracing actor on client */
    UFUNCTION(Client, Reliable)
    void SetTracingActor_Client(AActor* NewOwner);

private:
    /** Component to trace from */
    UPROPERTY()
    USceneComponent* VectorComponent;

    /** Cached tracing actor */
    UPROPERTY()
    AActor* TracingActor;

    /** Position offset from component */
    FVector PositionOffset;

    /** Rotation offset from component */
    FRotator RotationOffset;

    /** Initial launch velocity for arc */
    FVector LaunchVelocity;

    float SimulationDistance;

    /** Projectile radius for sweep */
    float ProjectileRadius;

    /** Gravity override */
    float OverrideGravityZ;

    /** Mode (single or continuous) */
    EArcTraceMode Mode;

    /** Trace channel */
    ECollisionChannel Channel;

    /** Interval between traces */
    float Interval;

    /** Whether task is active */
    bool bIsRunning;

    /** Whether to draw debug */
    bool bDrawDebug;

    /** Handle for timer */
    FTimerHandle TraceTimerHandle;
};
