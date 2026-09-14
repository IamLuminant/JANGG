// Copyright 2025, Luminant. All Rights Reserved.

#include "LuminantStatics.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "LE_ReplicatedObject.h"

void ULuminantStatics::PrintScreen(const FString& Text, const FColor Color, const float Duration)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Text);
	}
}

AActor* ULuminantStatics::SpawnActorFromClass(
	UObject* Outer,
	TSubclassOf<AActor> ActorClass,
	const FVector SpawnLocation,
	const FRotator SpawnRotation)
{
	if (!Outer || !ActorClass)
	{
		return nullptr;
	}

	UWorld* World = nullptr;

	// Try to get world from AActor
	if (AActor* OuterActor = Cast<AActor>(Outer))
	{
		World = OuterActor->GetWorld();
	}
	// Try to get world from UActorComponent
	else if (UActorComponent* OuterComponent = Cast<UActorComponent>(Outer))
	{
		World = OuterComponent->GetWorld();
	}
	// Try to get world from ULE_ReplicatedObject
	else if (ULE_ReplicatedObject* OuterReplicatedObject = Cast<ULE_ReplicatedObject>(Outer))
	{
		if (AActor* OwningActor = OuterReplicatedObject->GetOwningActor())
		{
			World = OwningActor->GetWorld();
		}
	}

	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Cast<AActor>(Outer);

	return World->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, SpawnParams);
}