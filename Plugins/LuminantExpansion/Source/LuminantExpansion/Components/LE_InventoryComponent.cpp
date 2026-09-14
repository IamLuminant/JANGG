// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_InventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "LE_StructsAndEnums.h"
#include "GameFramework/Pawn.h"
#include "LE_ItemObject.h"
#include "LE_ItemSlotObject.h"
#include "LE_ItemActor.h"
#include "LE_InteractionInterface.h"
#include "LE_AbilityComponent.h"

#pragma region Defaults
// Sets default values for this component's properties
ULE_InventoryComponent::ULE_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}
#pragma endregion

#pragma region Core Functions
void ULE_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULE_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) 
{ 
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction); 
}
#pragma endregion

#pragma region Replication
//Replication functions
void ULE_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULE_InventoryComponent, ItemSlots);
	DOREPLIFETIME(ULE_InventoryComponent, InventorySlotStatesArray);
}
bool ULE_InventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	return bWroteSomething;
}
#pragma endregion

#pragma region Initialization
//Initialize the inventory
void ULE_InventoryComponent::InitializeInventoryOnServer_Implementation()
{
	// Initialize the inventory size and state
	if (GetOwner())
	{
		//Initialize the inventory state and size
		for (int i = 0; i < InventorySize.X; i++)
		{
			for (int j = 0; j < InventorySize.Y; j++)
			{
				InventoryState.Add(FVector2D(i, j), false);
			}
		}
	}
}
bool ULE_InventoryComponent::InitializeInventoryOnServer_Validate() { return true; }
#pragma endregion

#pragma region Inventory Events
void ULE_InventoryComponent::OnInventoryRefreshed_Implementation()
{
}
#pragma endregion

#pragma region Inventory Functions
//Refresh the inventory state and widget on the server from anywhere
void ULE_InventoryComponent::RefreshInventoryOnServer_Implementation()
{
	if (GetOwner()) //if the component is on an actor
	{
		for (auto& InventoryStatePosition : InventoryState)
		{
			InventoryStatePosition.Value = false; //reset the inv state
		}
		for (int i = 0; i < ItemSlots.Num(); i++)
		{
			if (ItemSlots[i]->Item) //if there is an item in the slot
			{
				for (int j = 0; j < ItemSlots[i]->Item->CurrentShape.Num(); j++)
				{
					InventoryState[ItemSlots[i]->Item->Position + ItemSlots[i]->Item->CurrentShape[j]] = true; //set the occupied slots
				}
			}
			else
			{
				ItemSlots[i]->bIsValidSlot = false; //mark the slot as invalid
				ItemSlots.RemoveAt(i);
			}
		}
		if (!Cast<APawn>(GetOwner())->IsLocallyControlled())
		{
			InventorySlotStatesArray.Empty();
			for (auto& InventoryStatePosition : InventoryState)
			{ //replicate the inventory state
				F_InvSlot InventorySlot = F_InvSlot();
				InventorySlot.Position = InventoryStatePosition.Key;
				InventorySlot.bIsOccupied = InventoryStatePosition.Value;
				InventorySlotStatesArray.Add(InventorySlot); //this will call the onrep function to refresh the widget on the client
			}
			//print the inventory state amount
		}
		else
		{
			OnInventoryRefreshed();
		}
	}
}
bool ULE_InventoryComponent::RefreshInventoryOnServer_Validate() { return true; }

//Start looking for a place to put the item in all positions and find similar items, remember the previous owner to return the item if no place is found
void ULE_InventoryComponent::TryAddItemRandom_Implementation(ULE_ItemObject* Item, ULE_InventoryComponent* PrevOwner)
{
	if (GetOwner()) 
	{
		//search through all the items to find a similar item to stack with
		for (int i = 0; i < ItemSlots.Num(); i++)
		{
			//check if the item is the same type and can be stacked
			if (ItemSlots[i]->Item->GetClass() == Item->GetClass() && ItemSlots[i]->Item->MaxStackSize > 1)
			{
				AddItemToItem(Item, ItemSlots[i], PrevOwner);
				return;
			}
		}
		//if no similar item is found, try to add the item in all rotations
		TryAddItemInAllRotations(Item, PrevOwner);
	}
}
bool ULE_InventoryComponent::TryAddItemRandom_Validate(ULE_ItemObject* Item, ULE_InventoryComponent* PrevOwner) { return true; }

//try to add the item in all rotations
void ULE_InventoryComponent::TryAddItemInAllRotations(ULE_ItemObject* NewItem, ULE_InventoryComponent* PrevOwner)
{
	if (GetOwner())
	{
		//try to add the item in all rotations and shape
			//start with the default rotation
		NewItem->Rotation = E_Rotation::Up;
			//set the current shape to the default shape
		NewItem->CurrentShape = NewItem->DefaultShape;
		//try to find a place for the item in all rotations
		if (FindFirstEmptySlot(NewItem) != FVector2D(-1, -1))
		{
			AddItemAtPosition(NewItem, FindFirstEmptySlot(NewItem));
			return;
		}
		RotateItem(NewItem);
		if (FindFirstEmptySlot(NewItem) != FVector2D(-1, -1))
		{
			AddItemAtPosition(NewItem, FindFirstEmptySlot(NewItem));
			return;
		}
		RotateItem(NewItem);
		if (FindFirstEmptySlot(NewItem) != FVector2D(-1, -1))
		{
			AddItemAtPosition(NewItem, FindFirstEmptySlot(NewItem));
			return;
		}
		RotateItem(NewItem);
		if (FindFirstEmptySlot(NewItem) != FVector2D(-1, -1))
		{
			AddItemAtPosition(NewItem, FindFirstEmptySlot(NewItem));
			return;
		}
		//if no place is found, return the item to the previous owner or spawn it in the world
		if (PrevOwner)
		{
			PrevOwner->TryAddItemRandom(NewItem, nullptr);
		}
		else
		{
			NewItem->SpawnItem(GetOwner());
		}
	}
}

//Find the first empty slot in the inventory starting from top left
FVector2D ULE_InventoryComponent::FindFirstEmptySlot(ULE_ItemObject* Item)
{
	//search through all slots
	for (int i = 0; i < InventorySize.X; i++)
	{
		for (int j = 0; j < InventorySize.Y; j++)
		{
			bool bCanPlace = true;
			//check all the slots the item would occupy
			for (int k = 0; k < Item->CurrentShape.Num(); k++)
			{
				//if any of the slots are occupied or out of bounds, cannot place the item here
				if (InventoryState.Contains(FVector2D(i, j) + Item->CurrentShape[k]))
				{
					if (InventoryState[FVector2D(i, j) + Item->CurrentShape[k]])
					{
						bCanPlace = false;
						break;
					}
				}
				else
				{
					bCanPlace = false;
					break;
				}
			}
			//if the item can be placed here, return the position
			if (bCanPlace)
			{
				return FVector2D(i, j);
			}
		}
	}
	return InvalidSlot;
}

//Rotate the item
void ULE_InventoryComponent::RotateItem_Implementation(ULE_ItemObject* Item)
{
	if (GetOwner())
	{
		TArray<FVector2D> RotatedShape;
		for (auto& Point : Item->CurrentShape)
		{
			// Rotating 90 degrees clockwise means (x, y) -> (y, -x)
			FVector2D RotatedPoint(-Point.Y, Point.X);
			RotatedShape.Add(RotatedPoint);
		}
		// Adjust points to ensure all values are positive
		float MinX = InventorySize.X;
		float MinY = InventorySize.Y;
		for (const FVector2D& Point : RotatedShape)
		{
			if (Point.X < MinX) MinX = Point.X;
			if (Point.Y < MinY) MinY = Point.Y;
		}
		// If MinX or MinY are negative, shift all points by the absolute minimum to make them positive
		for (FVector2D& Point : RotatedShape)
		{
			Point.X -= MinX;
			Point.Y -= MinY;
		}
		Item->Rotation = (E_Rotation)(((int)Item->Rotation + 1) % 4);
		Item->CurrentShape = RotatedShape;
		RefreshInventoryOnServer();
	}
}

//Add item to the inventory at a specific position
void ULE_InventoryComponent::AddItemAtPosition_Implementation(ULE_ItemObject* Item, FVector2D Position)
{
	Item->Position = Position;
	Item->PreviousInventory = this;

	// Add the item as a replicated subobject FIRST
	AddReplicatedSubObject(Item);

	ULE_ItemSlotObject* NewItemSlot = NewObject<ULE_ItemSlotObject>(this);
	Item->PreviousSlot = NewItemSlot;
	
	// Add the item slot as a replicated subobject
	AddReplicatedSubObject(NewItemSlot);
	NewItemSlot->OwningInventory = this;
	ItemSlots.Add(NewItemSlot);
	NewItemSlot->OnSlotUpdated.AddDynamic(this, &ULE_InventoryComponent::RefreshInventoryOnServer);
	NewItemSlot->AddItem(Item);
}
bool ULE_InventoryComponent::AddItemAtPosition_Validate(ULE_ItemObject* Item, FVector2D Position) { return true; }

//Add item to another item that has some room
void ULE_InventoryComponent::AddItemToItem_Implementation(ULE_ItemObject* Item, ULE_ItemSlotObject* TargetSlot, ULE_InventoryComponent* PrevOwner)
{
	//check if the item is the same type
	if (Item->GetClass() == TargetSlot->Item->GetClass())
	{
		//make sure the target item is in the inventory and not destroyed or weird
		if (ItemSlots.Contains(TargetSlot))
		{
			//check if the item can be stacked without exceeding the max stack size
			if (TargetSlot->Item->Quantity + Item->Quantity <= TargetSlot->Item->MaxStackSize)
			{
				//add
				TargetSlot->Item->Quantity += Item->Quantity;
				TargetSlot->UpdateItem(Item->Quantity);
				Item->BeginDestroy();
			}
			else
			{
				//reduce the quantity of the item being added
				Item->Quantity -= TargetSlot->Item->MaxStackSize - TargetSlot->Item->Quantity;
				//fill the target item to max stack size
				TargetSlot->Item->Quantity = TargetSlot->Item->MaxStackSize;
				TargetSlot->UpdateItem(0);
				//if the item could not be fully added, try to add it in all rotations
				TryAddItemInAllRotations(Item, PrevOwner);
			}
			RefreshInventoryOnServer();
			return;
		}
	}
	//if the item could not be fully added, try to add it in all rotations
	TryAddItemInAllRotations(Item, PrevOwner);
	RefreshInventoryOnServer();
}
bool ULE_InventoryComponent::AddItemToItem_Validate(ULE_ItemObject* Item, ULE_ItemSlotObject* TargetItem, ULE_InventoryComponent* PrevOwner) { return true; }
#pragma endregion

#pragma region OnReps
//Inventory OnReps
void ULE_InventoryComponent::OnRep_InventorySlotStatesArray()
{
	//Empty the inventorystate on the client
	InventoryState.Empty();
	//Rebuild the inventory state from the replicated array
	for (int i = 0; i < InventorySlotStatesArray.Num(); i++)
	{
		InventoryState.Add(InventorySlotStatesArray[i].Position, InventorySlotStatesArray[i].bIsOccupied);
	}
	OnInventoryRefreshed();
}
void ULE_InventoryComponent::OnRep_ItemSlots()
{
	//after the ItemSlots are replicated, refresh the inventory state
	RefreshInventoryOnServer();
}
#pragma endregion

#pragma region Utility Functions
//will be moved to the widget blueprint later
void ULE_InventoryComponent::CheckIfItemIsHoveredOverFreeSlotOrSameItem(ULE_ItemObject* Item, FVector2D Position, bool& bIsHoveredOverFreeSlot, bool& bIsHoveredOverSameItem) const
{
	bIsHoveredOverFreeSlot = true;
	bIsHoveredOverSameItem = false;
	for (int i = 0; i < Item->CurrentShape.Num(); i++)
	{
		if (InventoryState.Contains(Position + Item->CurrentShape[i]))
		{
			if (InventoryState[Position + Item->CurrentShape[i]])
			{
				bIsHoveredOverFreeSlot = false;
				break;
			}
		}
		else
		{
			bIsHoveredOverFreeSlot = false;
			break;
		}
	}
	if (!bIsHoveredOverFreeSlot)
	{
		for (int i = 0; i < ItemSlots.Num(); i++)
		{
			if (ItemSlots[i] && ItemSlots[i]->Item)
			{
				// Check if both items are the same exact derived class
				if (ItemSlots[i]->Item->GetClass() == Item->GetClass())
				{
					if (ItemSlots[i]->Item->Position == Position)
					{
						if (ItemSlots[i]->Item->Quantity < ItemSlots[i]->Item->MaxStackSize)
						{
							bIsHoveredOverSameItem = true;
						}
						break;
					}
				}
			}
		}
	}
}
#pragma endregion