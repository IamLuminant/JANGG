// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "LE_InteractionComponent.generated.h"

class UCameraComponent;
class UBoxComponent;
class ALE_InteractionActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType, Category = "Interaction|Interaction")
class LUMINANTEXPANSION_API ULE_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULE_InteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	USceneComponent* StartingPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	ALE_InteractionActor* FocusedInteractionArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	AActor* FocusedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	bool bCanTrace;

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void Trace();

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void TraceInteractionArea(bool Trace);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryToInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StartInteract(ALE_InteractionActor* Int);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Interaction")
	void InterfaceFunction(ALE_InteractionActor* Int);


	//These are for UI implementation mostly
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trace")
	void StartedTracing(ALE_InteractionActor* Int);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trace")
	void StoppedTracing();

private:

	UPROPERTY(EditAnywhere, Category = "Trace")
	bool bIsTracingInteractionArea;

	UPROPERTY(EditAnywhere, Category = "Trace")
	float InteractionDuration;

		
};
