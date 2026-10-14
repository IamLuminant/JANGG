// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActorCPP.h"
#include "PlantBedCPP.generated.h"

class APlantClassCPP;
/**
 * 
 */
UCLASS()
class JANGG_API APlantBedCPP : public AInteractableActorCPP
{
	GENERATED_BODY()

public:

	APlantBedCPP();

	UPROPERTY(BlueprintReadOnly, Category = "Plant")
	APlantClassCPP* Plant = nullptr;

	virtual void InterfaceInteract_Implementation(ACharacter* interactor);

	UChildActorComponent* PlantChildActor = nullptr;
	
};
