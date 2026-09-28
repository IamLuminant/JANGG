// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActorCPP.h"

// Sets default values
AInteractableActorCPP::AInteractableActorCPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableActorCPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActorCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

