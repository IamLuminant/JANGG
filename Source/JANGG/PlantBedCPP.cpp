// Fill out your copyright notice in the Description page of Project Settings.


#include "PlantBedCPP.h"
#include "Gameframework/Character.h"
#include "FarmingComponentCPP.h"
#include "PlantClassCPP.h"

APlantBedCPP::APlantBedCPP()
{
	Super::StaticClass();

	PlantChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("PlantChildActor"));
}

void APlantBedCPP::InterfaceInteract_Implementation(ACharacter* interactor)
{
	UFarmingComponentCPP* farmComp = interactor->GetComponentByClass<UFarmingComponentCPP>();
	if (farmComp)
	{
		if (farmComp->CurrentItem->GetClass() == APlantClassCPP::StaticClass())
		{
			PlantChildActor->SetChildActorClass(farmComp->CurrentItem->GetClass());
		}
		//else if (farmComp->CurrentItem->GetClass() == )
	}
}

