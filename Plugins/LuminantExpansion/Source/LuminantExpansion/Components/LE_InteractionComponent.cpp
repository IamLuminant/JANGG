// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "LE_InteractionActor.h"
#include "LE_InteractionInterface.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

// Sets default values for this component's properties
ULE_InteractionComponent::ULE_InteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.15f;
}
// Called when the game starts
void ULE_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called every frame
void ULE_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanTrace)
	{
		Trace();
	}
}

void ULE_InteractionComponent::Trace()
{
	FHitResult Hit;
	FVector Start = StartingPoint->GetComponentLocation();
	FVector End = Start + (StartingPoint->GetForwardVector() * 1000.0f);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor() != FocusedActor)
		{
			FocusedActor = Hit.GetActor();
			TraceInteractionArea(Hit.GetActor()->GetClass() == ALE_InteractionActor::StaticClass());
		}
	}
	else
	{
		TraceInteractionArea(false);
	}
}

void ULE_InteractionComponent::TraceInteractionArea(bool Trace)
{
	if (Trace)
	{
		if (bIsTracingInteractionArea)
		{
			FocusedInteractionArea->ToggleFocus(false);
			StoppedTracing();
		}
		FocusedInteractionArea = Cast<ALE_InteractionActor>(FocusedActor);
		FocusedInteractionArea->ToggleFocus(true);
		bIsTracingInteractionArea = true;
		StartedTracing(FocusedInteractionArea);
	}
	else
	{
		if (bIsTracingInteractionArea)
		{
			bIsTracingInteractionArea = false;
			if (FocusedInteractionArea)
			{
				FocusedInteractionArea->ToggleFocus(false);
			}
			FocusedActor = nullptr;
			FocusedInteractionArea = nullptr;
			StoppedTracing();
		}
	}
}

void ULE_InteractionComponent::TryToInteract()
{
	if (bIsTracingInteractionArea)
	{
		InteractionDuration = FocusedInteractionArea->InteractionDuration;
		if (InteractionDuration <= 0.0f)
		{
			StartInteract(FocusedInteractionArea);
		}
	}
}

void ULE_InteractionComponent::StartInteract(ALE_InteractionActor* Int)
{
	if (Int)
	{
		if (Int->GetParentActor()->Implements<ULE_InteractionInterface>())
		{
			InterfaceFunction(Int);	
		}
	}
}

void ULE_InteractionComponent::InterfaceFunction_Implementation(ALE_InteractionActor* Int)
{
	if (Int)
	{
		ILE_InteractionInterface::Execute_InterfaceInteract(Int->GetParentActor(), Cast<ACharacter>(GetOwner()));
	}
}
bool ULE_InteractionComponent::InterfaceFunction_Validate(ALE_InteractionActor* Int)
{
	return true;
}

void ULE_InteractionComponent::StartedTracing_Implementation(ALE_InteractionActor* Int)
{
	
}

void ULE_InteractionComponent::StoppedTracing_Implementation()
{
}

