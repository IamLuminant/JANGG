// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "LE_StructsAndEnums.generated.h"



class ULE_ItemObject;

UENUM(BlueprintType)
enum class E_Rotation : uint8
{
	Up UMETA(DisplayName = "Up"),
	Right UMETA(DisplayName = "Right"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left")
};
USTRUCT(BlueprintType)
struct F_InvSlot
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FVector2D Position = FVector2D(0, 0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bIsOccupied = false;

};