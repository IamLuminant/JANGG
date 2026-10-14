// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActorCPP.h"
#include "GameFramework/Controller.h"
#include "LE_InteractionActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AInteractableActorCPP::AInteractableActorCPP()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(MeshComponent);
    MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
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

void AInteractableActorCPP::InterfaceToggleFocus_Implementation(bool Interactor)
{
    if (Interactor)
    {
        MeshComponent->SetOverlayMaterial(OverlayMaterial);
    }
    else
    {
        MeshComponent->SetOverlayMaterial(nullptr);
    }
}

