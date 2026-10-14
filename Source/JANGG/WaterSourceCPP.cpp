// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterSourceCPP.h"
#include "Gameframework/Character.h"
#include "FarmingComponentCPP.h"
#include "WateringCan.h"

void AWaterSourceCPP::InterfaceInteract_Implementation(ACharacter* interactor)
{
	UFarmingComponentCPP* farmComp = interactor->GetComponentByClass<UFarmingComponentCPP>();
	if (farmComp && farmComp->CurrentItem)
	{
		AWateringCan* wateringCan = Cast<AWateringCan>(farmComp->CurrentItem);
		if (wateringCan)
		{ 
			wateringCan->bIsFull = true;
		}
	}
}

