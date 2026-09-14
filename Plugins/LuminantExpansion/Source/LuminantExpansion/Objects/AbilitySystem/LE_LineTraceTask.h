// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/EngineTypes.h"
#include "LE_AbilityTask.h"
#include "LE_LineTraceTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLineTraceHitSignature, const FHitResult&, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLineTraceStoppedSignature);

class ULE_AbilityObject;

UENUM(BlueprintType)
enum class ELineTraceMode : uint8
{
	Single,
	Continuous
};

UCLASS()
class LUMINANTEXPANSION_API ULE_LineTraceTask : public ULE_AbilityTask
{
	GENERATED_BODY()

public:
    // Task creation function
	UFUNCTION(BlueprintCallable, Category = "Tasks", meta = (
        DefaultToSelf = "AbilityObject", 
        AdvancedDisplay = "AbilityObject, PositionOffset, RotationOffset, TraceChannel", 
        BlueprintInternalUseOnly = "TRUE"))
    static ULE_LineTraceTask* CreateLineTraceTask(
		UPARAM(meta = (NotAllowNone)) ULE_AbilityObject* AbilityObject = nullptr,
        UPARAM(meta = (NotAllowNone)) USceneComponent* VectorComponent = nullptr,
		FVector PositionOffset = FVector::ZeroVector,
		FRotator RotationOffset = FRotator::ZeroRotator,
		float Length = 1000.0f,
		ELineTraceMode Mode = ELineTraceMode::Single,
        float Interval = 0.1f,
        ECollisionChannel TraceChannel = ECC_Visibility,
		bool bDrawDebug = false
    );

    // Function to stop continuous tracing
    UFUNCTION(BlueprintCallable, Category = "Tasks")
    void StopTrace();

    // Delegates
    UPROPERTY(BlueprintAssignable)
    FLineTraceHitSignature OnHit;

    UPROPERTY(BlueprintAssignable)
    FLineTraceHitSignature OnMiss;

    UPROPERTY(BlueprintAssignable)
    FLineTraceStoppedSignature OnTraceStopped;

protected:
    virtual void Activate() override;

private:

	UFUNCTION(Server, Reliable, WithValidation)
	void SetTracingActor_Server(ULE_AbilityObject* StartObject);

	UFUNCTION(Client, Reliable)
	void SetTracingActor_Client(AActor* NewOwner);


    void PerformTrace();
    void ScheduleNextTrace();
	virtual void StopTask() override;

    FTimerHandle TraceTimerHandle;
    AActor* TracingActor;
	USceneComponent* VectorComponent;
	FVector PositionOffset;
	FRotator RotationOffset;
    float Length;
    float Interval;
    ELineTraceMode Mode;
    TEnumAsByte<ECollisionChannel> Channel;
    bool bIsRunning;
	bool bDrawDebug;
};
