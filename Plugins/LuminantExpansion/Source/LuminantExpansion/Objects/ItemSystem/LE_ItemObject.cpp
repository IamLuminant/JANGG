// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_ItemObject.h"
#include "LE_ItemActor.h"
#include "LE_InteractionActor.h"
#include "LE_InventoryComponent.h"
#include "LE_AbilityComponent.h"
#include "LuminantStatics.h"
#include "LE_StructsAndEnums.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

#pragma region Replication
void ULE_ItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Replicate all item properties
	DOREPLIFETIME(ULE_ItemObject, Quantity);
	DOREPLIFETIME(ULE_ItemObject, Position);
	DOREPLIFETIME(ULE_ItemObject, CurrentShape);
	DOREPLIFETIME(ULE_ItemObject, Rotation);
	DOREPLIFETIME(ULE_ItemObject, PreviousInventory);
	DOREPLIFETIME(ULE_ItemObject, PreviousSlot);
	DOREPLIFETIME(ULE_ItemObject, Name);
	DOREPLIFETIME(ULE_ItemObject, Description);
	DOREPLIFETIME(ULE_ItemObject, Icon);
	DOREPLIFETIME(ULE_ItemObject, Mesh);
	DOREPLIFETIME(ULE_ItemObject, ItemActorClass);
	DOREPLIFETIME(ULE_ItemObject, DefaultShape);
	DOREPLIFETIME(ULE_ItemObject, MaxStackSize);
	DOREPLIFETIME(ULE_ItemObject, SpawnedActor);
	DOREPLIFETIME(ULE_ItemObject, SpawnedMeshComponent);
}
#pragma endregion

#pragma region Item Functions
void ULE_ItemObject::ConvertShapeToCoordinatesByTilePosition(FVector2D TopLeft, TArray<FVector2D>& OutPos)
{
	//Initialize the array of positions
	TArray<FVector2D> Positions;

	//Iterate over each point in the item's shape
	for (const FVector2D& Point : CurrentShape)
	{
		//Calculate the position by adding the point to the top left corner
		FVector2D NewPosition = TopLeft + Point;
		Positions.Add(NewPosition);
	}

	//Return the array of positions
	OutPos = Positions;
}

void ULE_ItemObject::SpawnItem_Implementation(AActor* Owner)
{
	//Verify owner exists
	if (Owner)
	{
		//Calculate spawn location in front of the owner
		FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 300;
		FRotator SpawnRotation = FRotator(0, 0, 0);
		
		//Spawn the item actor
		ALE_ItemActor* NewItem = Cast<ALE_ItemActor>(ULuminantStatics::SpawnActorFromClass(this, ALE_ItemActor::StaticClass(), SpawnLocation, SpawnRotation));
		if (NewItem)
		{
			//Set the item object and mesh
			NewItem->ItemObject = this;
			NewItem->Mesh->SetStaticMesh(Mesh);
			
			//Set the interaction actor's mesh
			ALE_InteractionActor* IntActor = Cast<ALE_InteractionActor>(NewItem->ChildInteractionActor->GetChildActor());
			if (IntActor)
			{
				IntActor->ColliderShape->SetStaticMesh(Mesh);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner or ItemClass is null"));
	}
}
bool ULE_ItemObject::SpawnItem_Validate(AActor* Owner) { return true; }
#pragma endregion

#pragma region Selection
void ULE_ItemObject::InterfaceSelectItem_Implementation()
{
	//Call server select function
	ServerSelect();
}

void ULE_ItemObject::InterfaceDeselectItem_Implementation()
{
	//Call server deselect function
	ServerDeselect();
}

void ULE_ItemObject::ServerSelect_Implementation()
{
	//Get ability component from inventory owner
	ULE_AbilityComponent* AbilityComponent = PreviousInventory->GetOwner()->FindComponentByClass<ULE_AbilityComponent>();
	if (AbilityComponent)
	{
		//Spawn and attach the item
		SpawnItemAndAttach(bUseActorOnSocket);
	}
}
bool ULE_ItemObject::ServerSelect_Validate() { return true; }

void ULE_ItemObject::ServerDeselect_Implementation()
{
	//Destroy and detach the item
	DestroyItemAndDetach();
}
bool ULE_ItemObject::ServerDeselect_Validate() { return true; }

void ULE_ItemObject::SpawnItemAndAttach_Implementation(bool bUseActor)
{
	//Decide whether to use actor or mesh component
	if (bUseActor)
	{
		//Spawn and attach an actor if not already spawned
		if (ItemActorClass && SpawnedActor == nullptr)
		{
			FVector SpawnLocation = FVector(0, 0, 0);
			FRotator SpawnRotation = FRotator(0, 0, 0);
			SpawnedActor = Cast<ALE_ItemActor>(ULuminantStatics::SpawnActorFromClass(this, ItemActorClass, SpawnLocation, SpawnRotation));
			if (SpawnedActor)
			{
				SpawnedActor->SetReplicates(true);
				SpawnedActor->AttachToActor(PreviousInventory->GetOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
			}
		}
	}
	else
	{
		//Create and attach a mesh component if not already created
		if (SpawnedMeshComponent == nullptr && Mesh)
		{
			SpawnedMeshComponent = NewObject<UStaticMeshComponent>(PreviousInventory->GetOwner(), UStaticMeshComponent::StaticClass());
			if (SpawnedMeshComponent)
			{
				SpawnedMeshComponent->RegisterComponent();
				SpawnedMeshComponent->SetIsReplicated(true);
				SpawnedMeshComponent->SetStaticMesh(Mesh);
				bool worked = SpawnedMeshComponent->AttachToComponent(Cast<ACharacter>(PreviousInventory->GetOwner())->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
			}
		}
	}
}

void ULE_ItemObject::DestroyItemAndDetach_Implementation()
{
	//Destroy the spawned actor if it exists
	if (SpawnedActor)
	{
		SpawnedActor->Destroy();
	}
	
	//Destroy the spawned mesh component if it exists
	if (SpawnedMeshComponent)
	{
		SpawnedMeshComponent->SetStaticMesh(nullptr);
		SpawnedMeshComponent->DestroyComponent();
		SpawnedMeshComponent = nullptr;
	}
}
#pragma endregion


