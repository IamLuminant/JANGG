// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LE_ReplicatedObject.h"
#include "Delegates/Delegate.h"		
#include "LE_ItemSlotObject.generated.h"

#pragma region Forward Declarations
class ULE_ItemObject;
class ULE_InventoryComponent;
#pragma endregion

//Declare the multicast delegate type for slot updates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotUpdatedEvent);

UCLASS()
class LUMINANTEXPANSION_API ULE_ItemSlotObject : public ULE_ReplicatedObject
{
	GENERATED_BODY()

#pragma region Replication
protected:
	//Get lifetime replicated properties for networking
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region Item Slot Properties
public:
	//The item object stored in this slot
	UPROPERTY(ReplicatedUsing = OnRep_Item, EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
	ULE_ItemObject* Item = nullptr;

	//OnRep callback when item is replicated
	UFUNCTION()
	void OnRep_Item();

	//The inventory component that owns this slot
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
	ULE_InventoryComponent* OwningInventory = nullptr;

	//Event broadcast when the slot is updated
	UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintReadWrite, Category = "Item Slot")
	FSlotUpdatedEvent OnSlotUpdated;

	//Whether this slot is valid (used internally)
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
	bool bIsValidSlot = true;
#pragma endregion

#pragma region Item Slot Functions
public:
	//Add an item to this slot
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Item Slot")
	void AddItem(ULE_ItemObject* NewItem);

	//Remove a quantity of items from this slot
	UFUNCTION(BlueprintNativeEvent, Category = "Item Slot")
	void RemoveItem(int amount);

	//Server RPC to remove items from this slot
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Item Slot")
	void RemoveItemServer(int amount);

	//Update the item quantity in this slot
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Item Slot")
	void UpdateItem(int amount);
#pragma endregion
};
