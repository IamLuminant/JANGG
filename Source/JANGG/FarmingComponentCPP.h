// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FarmingComponentCPP.generated.h"

class APlantClassCPP;

UENUM(BlueprintType)
enum class EPlantType : uint8
{
	Small,
	Medium,
	Large,
};

UENUM(BlueprintType)
enum class EItem : uint8
{
	Plant, 
	WateringCan,
	Weapon,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JANGG_API UFarmingComponentCPP : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFarmingComponentCPP();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentItem = nullptr;
};
