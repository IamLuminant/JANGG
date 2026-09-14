// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "LE_StructsAndEnums.h"
#include "LE_InventoryComponent.generated.h"

#pragma region Forward Declarations
class ULE_ItemObject;
class ULE_ItemSlotObject;
enum class E_ItemType : uint8;
struct F_InvSlot;
#pragma endregion

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType, Category = "Inventory|Inventory")
class LUMINANTEXPANSION_API ULE_InventoryComponent : public UActorComponent
{
#pragma region Defaults
	GENERATED_BODY()

public:	
	ULE_InventoryComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//Object replication functions
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	UFUNCTION(BlueprintCallable, Category = "Inventory|Inventory")
	void AddReplicatedObject(UObject* ObjectToAdd) { if (ObjectToAdd) { AddReplicatedSubObject(ObjectToAdd); } }
#pragma endregion
#pragma region Private
private:	
	//Invalid slot constant
	const FVector2D InvalidSlot = FVector2D(-1, -1);
#pragma endregion
#pragma region Inv Variables
public: //Inv Stats

	//length and width of the inventory grid
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Inventory")
	FVector2D InventorySize = FVector2D(10, 10);

	//Replicated Inventory item slots array
	UPROPERTY(ReplicatedUsing = OnRep_ItemSlots, EditAnywhere, BlueprintReadWrite, Category = "Inventory|Inventory")
	TArray<TObjectPtr<ULE_ItemSlotObject>> ItemSlots;
	UFUNCTION() void OnRep_ItemSlots();

	//Inventory slot state map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Inventory")
	TMap<FVector2D, bool> InventoryState;

	//Inventory State Replication
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventorySlotStatesArray, Category = "Inventory|Inventory")
	TArray<F_InvSlot> InventorySlotStatesArray;
	UFUNCTION() void OnRep_InventorySlotStatesArray();
#pragma endregion
#pragma region Inv Functions
public: //Inv Functions

	// Server function to initialize the inventory size and state
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|Inventory")
	void InitializeInventoryOnServer();

	//Refresh the inventory state and widget if server
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|Inventory")
	void RefreshInventoryOnServer();

	//Start looking for a place to put the item and find similar items
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|Inventory")
	void TryAddItemRandom(ULE_ItemObject* Item, ULE_InventoryComponent* PrevOwner);

	//try to add the item in all rotations and positions
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Inventory|Inventory")
	void TryAddItemInAllRotations(ULE_ItemObject* NewItem, ULE_InventoryComponent* PrevOwner);

	//Find the first empty slot in the inventory starting from top left
	UFUNCTION(BlueprintCallable, Category = "Inventory|Inventory")
	FVector2D FindFirstEmptySlot(ULE_ItemObject* Item);

	//Rotate the item
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|Inventory")
	void RotateItem(ULE_ItemObject* Item);

	//Add item to the inventory at a specific position
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|Inventory")
	void AddItemAtPosition(ULE_ItemObject* Item, FVector2D Position);

	//Add item to another item that has some room
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|Inventory")
	void AddItemToItem(ULE_ItemObject* Item, ULE_ItemSlotObject* TargetItem, ULE_InventoryComponent* PrevOwner);


	//Check if the item is hovered over a free slot or the same item UI ONLY
	UFUNCTION(BlueprintCallable, Category = "Inventory|Inventory")
	void CheckIfItemIsHoveredOverFreeSlotOrSameItem(ULE_ItemObject* Item, FVector2D Position, bool& bIsHoveredOverFreeSlot, bool& bIsHoveredOverSameItem) const;

protected:

	//Inventory refreshed event
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Inventory")
	void OnInventoryRefreshed();
#pragma endregion
};
