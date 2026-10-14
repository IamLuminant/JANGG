// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActorCPP.h"
#include "WaterSourceCPP.generated.h"

/**
 * 
 */
UCLASS()
class JANGG_API AWaterSourceCPP : public AInteractableActorCPP
{
	GENERATED_BODY()
public:

	virtual void InterfaceInteract_Implementation(ACharacter* interactor);
	
};
