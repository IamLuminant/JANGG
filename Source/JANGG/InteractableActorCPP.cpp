// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActorCPP.h"
#include "GameFramework/Controller.h"
#include "LE_InteractionActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AInteractableActorCPP::AInteractableActorCPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("InteractionActor"));
	InteractionActor->SetChildActorClass(ALE_InteractionActor::StaticClass());
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetStaticMesh(Mesh);
	SetRootComponent(MeshComponent);

	MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	ALE_InteractionActor* interaction = Cast<ALE_InteractionActor>(InteractionActor->GetChildActor());
	if (interaction)
	{
		interaction->ColliderMaterial = MeshColliderMaterial;
		interaction->ColliderShape = MeshComponent;
	}

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

void AInteractableActorCPP::InterfaceInteract_Implementation(ACharacter* Interactor)
{
	MeshComponent->SetOverlayMaterial(OverlayMaterial);
}

void AInteractableActorCPP::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (!Mesh || !MeshColliderMaterial || !MeshComponent)
    {
        return;
    }

    MeshComponent->SetStaticMesh(Mesh);

    if (!InteractionActor) return;

    ALE_InteractionActor* interaction = Cast<ALE_InteractionActor>(InteractionActor->GetChildActor());
    if (!interaction || !interaction->ColliderShape) return;

    UStaticMeshComponent* collider = interaction->ColliderShape;
    collider->SetStaticMesh(Mesh);
    collider->SetVisibility(bShowColliderMesh);
    collider->SetHiddenInGame(!bShowColliderMesh);

    const int32 NumSlots = Mesh->GetStaticMaterials().Num();
    for (int32 i = 0; i < NumSlots; ++i)
    {
        collider->SetMaterial(i, MeshColliderMaterial);
    }
}

