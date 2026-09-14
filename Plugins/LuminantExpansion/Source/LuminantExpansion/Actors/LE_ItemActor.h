// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "LE_InteractionInterface.h"
#include "LE_ItemActor.generated.h"

#pragma region Forward Declarations
class USphereComponent;
class UInventoryComponent;
class ULE_ItemObject;
class ULE_InventoryComponent;
#pragma endregion

UCLASS(Blueprintable, BlueprintType)
class LUMINANTEXPANSION_API ALE_ItemActor : public AActor, public ILE_InteractionInterface
{
	GENERATED_BODY()

#pragma region Defaults
public:	
	//Sets default values for this actor's properties
	ALE_ItemActor();

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region Components
public:
	//Static mesh component for the item's visual representation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	//Child actor component for interaction functionality
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* ChildInteractionActor;
#pragma endregion

#pragma region Item Properties
public:
	//The item object this actor represents
	UPROPERTY(BlueprintReadWrite, Category = "Components")
	ULE_ItemObject* ItemObject;

	//Class of the item to spawn on initialization
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<ULE_ItemObject> StartupItemClass;

	//Starting quantity of the item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int StartingQuantity = 1;
#pragma endregion

#pragma region Item Functions
public:
	//Server RPC to handle item interaction
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Item")
	void ItemInteractOnServer(ACharacter* Interactor);

	//Set default properties for the item when picked up
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Item")
	void SetObjectDefaults(ULE_ItemObject* Item, ULE_InventoryComponent* Inventory);

	//Client RPC to highlight or unhighlight the item
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Item")
	void HighlightItem(bool bHighlight);

	//Interface implementation for interaction
	virtual void InterfaceInteract_Implementation(ACharacter* Interactor) override;

	//Interface implementation for focus toggle
	virtual void InterfaceToggleFocus_Implementation(bool Interactor) override;
#pragma endregion
};
