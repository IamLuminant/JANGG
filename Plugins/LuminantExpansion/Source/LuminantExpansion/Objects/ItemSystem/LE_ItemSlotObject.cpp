// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_ItemSlotObject.h"
#include "LE_ItemObject.h"
#include "Net/UnrealNetwork.h"
#include "LE_InventoryComponent.h"
#include "GameFramework/Pawn.h"

#pragma region Replication
void ULE_ItemSlotObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Replicate all slot properties
	DOREPLIFETIME(ULE_ItemSlotObject, Item);
	DOREPLIFETIME(ULE_ItemSlotObject, bIsValidSlot);
	DOREPLIFETIME(ULE_ItemSlotObject, OwningInventory);
}
#pragma endregion

#pragma region Item Slot Functions
void ULE_ItemSlotObject::OnRep_Item()
{
	//Check if the inventory exists
	if (!OwningInventory)
		return;

	//Get the owner pawn
	APawn* OwnerPawn = Cast<APawn>(OwningInventory->GetOwner());
	if (!OwnerPawn)
		return;

	//Update the item on locally controlled clients
	if (OwnerPawn->IsLocallyControlled())
		UpdateItem(0);
}

void ULE_ItemSlotObject::AddItem_Implementation(ULE_ItemObject* NewItem)
{
	//Verify the item exists
	if (NewItem)
	{
		//If the slot is empty, add the item
		if (Item == nullptr)
		{
			Item = NewItem;
			Item->PreviousSlot = this;
			//Check if the inventory exists
			if (!OwningInventory)
				return;

			//Get the owner pawn
			APawn* OwnerPawn = Cast<APawn>(OwningInventory->GetOwner());
			if (!OwnerPawn)
				return;

			//Update the item on locally controlled clients
			if (OwnerPawn->IsLocallyControlled())
				UpdateItem(0);
		}
		else
		{
			//If the slot is full, try to add to the previous slot
			if (!NewItem->PreviousSlot || NewItem->PreviousSlot->Item || !NewItem->PreviousSlot->bIsValidSlot)
			{
				NewItem->PreviousSlot->AddItem(NewItem);
			}
			else
			{
				//Otherwise, try to add randomly to the inventory
				NewItem->PreviousInventory->TryAddItemRandom(NewItem, nullptr);
			}
		}
	}
}

void ULE_ItemSlotObject::RemoveItem_Implementation(int amount)
{
	//Remove the specified amount from the item
	if (Item)
	{
		int removedAmount = FMath::Min(amount, Item->Quantity);
		Item->Quantity -= removedAmount;
		
		//If the item quantity is zero or less, remove the item from the slot
		if (amount <= 0)
		{
			Item = nullptr;
			//Check if the inventory exists
			if (!OwningInventory)
				return;

			//Get the owner pawn
			APawn* OwnerPawn = Cast<APawn>(OwningInventory->GetOwner());
			if (!OwnerPawn)
				return;

			//Update the item on locally controlled clients
			if (OwnerPawn->IsLocallyControlled())
				UpdateItem(0);
		}
	}
}

void ULE_ItemSlotObject::RemoveItemServer_Implementation(int amount)
{
	//Call the native event implementation
	RemoveItem(amount);
}

void ULE_ItemSlotObject::UpdateItem_Implementation(int amount)
{
	//Update the item quantity
	if (Item)
		Item->Quantity += amount;
	
	//Broadcast the update event
	OnSlotUpdated.Broadcast();
}
#pragma endregion

