// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "LE_ReplicatedObject.h"
#include "LE_InteractionInterface.h"
#include "LE_ItemObject.generated.h"

#pragma region Forward Declarations
class ALE_ItemActor;
class ULE_InventoryComponent;
class ULE_ItemSlotObject;
enum class E_ItemType : uint8;
enum class E_Rotation : uint8;
#pragma endregion

UCLASS(Blueprintable, BlueprintType)
class LUMINANTEXPANSION_API ULE_ItemObject : public ULE_ReplicatedObject, public ILE_InteractionInterface
{
	GENERATED_BODY()

#pragma region Replication
protected:
	//Get lifetime replicated properties for networking
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region Inventory Properties
public:
	//Quantity of this item in the stack
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item|Inventory")
	int32 Quantity;

	//Position in the inventory grid
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item|Inventory")
	FVector2D Position = FVector2D(-1, -1);

	//Current shape/dimensions of the item based on rotation
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item|Inventory")
	TArray<FVector2D> CurrentShape;

	//Current rotation state of the item
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item|Inventory")
	E_Rotation Rotation;

	//Reference to the previous inventory this item was in
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item|Inventory")
	ULE_InventoryComponent* PreviousInventory;

	//Reference to the previous inventory slot this item was in
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item|Inventory")
	ULE_ItemSlotObject* PreviousSlot;
#pragma endregion

#pragma region Item Properties
public:
	//Display name of the item
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	FString Name;

	//Description of the item
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	FString Description;

	//UI icon for the item
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	UTexture2D* Icon;

	//3D mesh for the item
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	UStaticMesh* Mesh;

	//Actor class to spawn when the item is selected
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	TSubclassOf<AActor> ItemActorClass;

	//Socket name to attach the item to on the character
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item|Item")
	FName AttachSocketName = "item_socket";

	//Whether to use the actor class or mesh component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	bool bUseActorOnSocket = false;

	//Default shape/dimensions of the item when not rotated
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	TArray<FVector2D> DefaultShape;

	//Maximum quantity this item can stack to
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item")
	int32 MaxStackSize = 1;
#pragma endregion

#pragma region Item Functions
public:
	//Convert the item's shape to world coordinates based on a grid position
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ConvertShapeToCoordinatesByTilePosition(FVector2D TopLeft, TArray<FVector2D>& OutPos);

	//Spawn the item as a world actor that can be picked up
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	virtual void SpawnItem(AActor* Owner);
#pragma endregion

#pragma region Selection
protected:
	//The spawned actor when the item is selected
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item|Selection")
	AActor* SpawnedActor = nullptr;

	//The spawned mesh component when the item is selected
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item|Selection")
	UStaticMeshComponent* SpawnedMeshComponent = nullptr;

	//Interface implementation for item selection
	virtual void InterfaceSelectItem_Implementation() override;
	
	//Interface implementation for item deselection
	virtual void InterfaceDeselectItem_Implementation() override;

	//Server RPC to select the item
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSelect();

	//Server RPC to deselect the item
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeselect();

	//Spawn and attach the item to the character
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Item|Selection")
	void SpawnItemAndAttach(bool bUseActor);

	//Destroy and detach the item from the character
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Item|Selection")
	void DestroyItemAndDetach();
#pragma endregion
};
