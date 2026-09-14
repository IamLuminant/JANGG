// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_ItemActor.h"
#include "Components/ChildActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LE_InventoryComponent.h"
#include "LE_ItemObject.h"
#include "LE_StructsAndEnums.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "LE_InteractionActor.h"
#include "Engine/Engine.h"

#pragma region Defaults
ALE_ItemActor::ALE_ItemActor()
{
	//Enable ticking
	PrimaryActorTick.bCanEverTick = true;
	
	//Create and set root mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCustomDepthStencilValue(1);

	//Create child interaction actor component
	ChildInteractionActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildInteractionActor"));
	ChildInteractionActor->SetupAttachment(Mesh);
	ChildInteractionActor->SetChildActorClass(ALE_InteractionActor::StaticClass());

	//Setup replication
	bReplicates = true;
	ReplicatedComponents.Add(ChildInteractionActor);
	ReplicatedComponents.Add(Mesh);
	ChildInteractionActor->SetIsReplicated(true);
	Mesh->SetIsReplicated(true);

	//Setup physics and collision
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}
#pragma endregion

#pragma region Core Functions
// Called when the game starts or when spawned
void ALE_ItemActor::BeginPlay()
{
	Super::BeginPlay();

	//Initialize item object on server
	if (HasAuthority())
	{
		if (ItemObject == nullptr && StartupItemClass)
		{
			ItemObject = NewObject<ULE_ItemObject>(this, StartupItemClass);
			ItemObject->Quantity = StartingQuantity;
		}
	}
}

// Called every frame
void ALE_ItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion

#pragma region Item Functions
void ALE_ItemActor::ItemInteractOnServer_Implementation(ACharacter* Interactor)
{
	//Verify authority and valid interactor
	if (HasAuthority() && Interactor)
	{
		//Get the interactor's inventory component
		ULE_InventoryComponent* Inventory = Cast<ULE_InventoryComponent>(Interactor->GetComponentByClass(ULE_InventoryComponent::StaticClass()));
		if (Inventory)
		{
			if (ItemObject)
			{
				//Set defaults and add to inventory
				SetObjectDefaults(ItemObject, Inventory);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ItemObject is null"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ItemObject is null"));
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ItemActor Interacted but no authority"));
	}
}

void ALE_ItemActor::SetObjectDefaults_Implementation(ULE_ItemObject* Item, ULE_InventoryComponent* Inventory)
{
	//Duplicate the item for the inventory owner
	ULE_ItemObject* NewItem = Cast<ULE_ItemObject>(StaticDuplicateObject(Item, Inventory->GetOwner()));
	NewItem->Rotation = E_Rotation::Up;
	NewItem->PreviousInventory = nullptr;
	//Add the item to the inventory and destroy the actor
	Inventory->TryAddItemRandom(NewItem, nullptr);
	Destroy();
}

void ALE_ItemActor::HighlightItem_Implementation(bool bHighlight)
{
	//Toggle custom depth rendering for highlight effect
	if (bHighlight)
	{
		Mesh->SetRenderCustomDepth(true);
	}
	else
	{
		Mesh->SetRenderCustomDepth(false);
	}
}
#pragma endregion

#pragma region Interface Implementation
void ALE_ItemActor::InterfaceInteract_Implementation(ACharacter* Interactor)
{
	//Call the server interaction function
	ItemInteractOnServer(Interactor);
}

void ALE_ItemActor::InterfaceToggleFocus_Implementation(bool Interactor)
{
	//Toggle the highlight on the item
	HighlightItem(Interactor);
}
#pragma endregion

