// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "LE_ReplicatedObject.generated.h"

UCLASS(Blueprintable, BlueprintType)
class LUMINANTEXPANSION_API ULE_ReplicatedObject : public UObject
{
	GENERATED_BODY()

#pragma region Core Functions
public:
	//Get the owning actor of this replicated object
	UFUNCTION(BlueprintPure, Category = "Luminant's Expansions")
	AActor* GetOwningActor() const;

	//Override GetWorld to work with replicated objects
	UFUNCTION(BlueprintCallable, Category = "Luminant's Expansions")
	virtual UWorld* GetWorld() const override;
#pragma endregion

#pragma region Replication
protected:
	//Enable networking for this object
	virtual bool IsSupportedForNetworking() const override { return true; }

	//Determine which machines should execute a function call
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;

	//Get lifetime replicated properties for networking
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Handle remote function calls
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	
	//Handle remote function calls from another object
	virtual bool CallRemoteFunction(UObject* InObj, UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack);
#pragma endregion
};
