// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_ReplicatedObject.h"
#include "Net/UnrealNetwork.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "GameFramework/Actor.h"
#include "Engine/NetDriver.h"

#pragma region Replication
void ULE_ReplicatedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Add Blueprint replication properties if this is a Blueprint generated class
	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass())) 
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
}

bool ULE_ReplicatedObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	
	//Get the owning actor
	AActor* Owner = GetOwningActor();
	if (!Owner) return false;

	//Get the net driver from the owner
	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (NetDriver)
	{
		//Process the remote function call
		NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

bool ULE_ReplicatedObject::CallRemoteFunction(UObject* InObj, UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	//Get the owning actor from the outer
	AActor* Owner = Cast<AActor>(GetOuter());
	if (!Owner) return false;

	//Get the net driver from the owner
	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (!NetDriver) return false;

	//Process the remote function call
	NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
	return true;
}
#pragma endregion

#pragma region Core Functions
AActor* ULE_ReplicatedObject::GetOwningActor() const
{
	//Get the actor that owns this object
	return GetTypedOuter<AActor>();
}

UWorld* ULE_ReplicatedObject::GetWorld() const
{
	//Get the world from the outer object
	if (const UObject* MyOuter = GetOuter())
	{
		return MyOuter->GetWorld();
	}
	return nullptr;
}

int32 ULE_ReplicatedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	check(GetOuter() != nullptr);
	//Get the function callspace from the outer object
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}
#pragma endregion


