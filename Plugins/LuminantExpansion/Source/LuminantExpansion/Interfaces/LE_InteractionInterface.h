// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LE_InteractionInterface.generated.h"

#pragma region Forward Declarations
class ULE_AbilityComponent;
#pragma endregion

//Interface class definition
UINTERFACE(MinimalAPI)
class ULE_InteractionInterface : public UInterface
{
	GENERATED_BODY()
};

#pragma region Interface Functions
class LUMINANTEXPANSION_API ILE_InteractionInterface
{
	GENERATED_BODY()

public:
	//Interaction
	//Called when the actor is interacted with
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InterfaceInteract(ACharacter* Interactor);

	//Called to toggle focus (highlight) on the actor
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InterfaceToggleFocus(bool Interactor);

	//Inventory
	//Called to open the inventory UI
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void OpenInventory();

	//Called to close the inventory UI
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void CloseInventory();

	//Item
	//Called to use/consume the item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void InterfaceUseItem(int ActionIndex = -1);

	//Called to stop using the item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void InterfaceStopUseItem(int ActionIndex = -1);

	//Called when the item is selected
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void InterfaceSelectItem();

	//Called when the item is deselected
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void InterfaceDeselectItem();
#pragma endregion
};
