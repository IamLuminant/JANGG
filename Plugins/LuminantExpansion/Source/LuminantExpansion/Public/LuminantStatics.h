// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LuminantStatics.generated.h"

/**
 * 
 */	
UCLASS()
class LUMINANTEXPANSION_API ULuminantStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void PrintScreen(const FString& Text, const FColor Color = FColor::Green, const float Duration = 5.f);

	UFUNCTION(BlueprintCallable, Category = "LuminantsExpansions", meta = (AdvancedDisplay = "Outer", DefaultToSelf = "Outer", DeterminesOutputType = "ActorClass"))
	static AActor* SpawnActorFromClass(
		UObject* Outer,
		TSubclassOf<AActor> ActorClass,
		const FVector SpawnLocation,
		const FRotator SpawnRotation
	);

};
