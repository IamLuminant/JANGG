// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_InteractionActor.h"
#include "LE_InteractionInterface.h"
#include "LE_ItemActor.h"
#include "Engine/Engine.h"

// Sets default values
ALE_InteractionActor::ALE_InteractionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ColliderShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ColliderShape"));
	RootComponent = ColliderShape;

	ColliderShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ColliderShape->SetCollisionResponseToAllChannels(ECR_Ignore);
	ColliderShape->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	ColliderShape->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1.01, 1.01, 1.01)));
}
void ALE_InteractionActor::BeginPlay()
{
	Super::BeginPlay();
	if (GetParentActor())
	{
		ALE_ItemActor* OwnerActor = Cast<ALE_ItemActor>(GetParentActor());
		if (OwnerActor)
		{
			if (!ColliderShape->GetStaticMesh())
			{
				ColliderShape->SetStaticMesh(OwnerActor->Mesh->GetStaticMesh());
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Parent Actor"));
		}
	}
	
}
void ALE_InteractionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALE_InteractionActor::ToggleFocus(bool F)
{
	Focused = F;
	if (GetParentActor())
	{
		if (GetParentActor()->Implements<ULE_InteractionInterface>())
		{
			ILE_InteractionInterface::Execute_InterfaceToggleFocus(GetParentActor(), F);
		}
	}
}

void ALE_InteractionActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Check if the mesh and material are valid
	if (ColliderMesh && ColliderMaterial)
	{
		ColliderShape->SetStaticMesh(ColliderMesh);

		// Apply the material to all sections of the mesh
		for (int32 i = 0; i < ColliderShape->GetNumMaterials(); i++)
		{
			ColliderShape->SetMaterial(i, ColliderMaterial);
		}
	}

	// Set visibility based on ShowCollider
	ColliderShape->SetVisibility(ShowCollider);
}

