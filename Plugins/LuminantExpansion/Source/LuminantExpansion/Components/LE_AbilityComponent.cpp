// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_AbilityComponent.h"
#include "LE_AbilityObject.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"

// Sets default values for this component's properties
ULE_AbilityComponent::ULE_AbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}
void ULE_AbilityComponent::BeginPlay()
{
	Super::BeginPlay();

}
void ULE_AbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULE_AbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULE_AbilityComponent, Abilities);
}
bool ULE_AbilityComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	return bWroteSomething;
}

ULE_AbilityObject* ULE_AbilityComponent::GrantAbility(TSubclassOf<ULE_AbilityObject> AbilityClass)
{
	if (AbilityClass && !Abilities.ContainsByPredicate([AbilityClass](const ULE_AbilityObject* Ability)
		{ return Ability->GetClass() == AbilityClass; }))
	{
		ULE_AbilityObject* Ability = NewObject<ULE_AbilityObject>(GetOwner(), AbilityClass.Get());
		if (Ability)
		{
			AddReplicatedSubObject(Ability);
			Ability->AbilityComponent = this; // Set the AbilityComponent reference
			Abilities.Add(Ability);
			Ability->ServerInitializeAbility();
			return Ability;
		}
		return nullptr;
	}
	return nullptr;
}

void ULE_AbilityComponent::RemoveAbility(TSubclassOf<ULE_AbilityObject> AbilityToRemove)
{
	if (AbilityToRemove)
	{
		for (int i = 0; i < Abilities.Num(); i++)
		{
			if (Abilities[i]->GetClass() == AbilityToRemove)
			{
				if (!Cast<APawn>(GetOwner())->IsLocallyControlled())
				{
					RemoveReplicatedSubObject(Abilities[i]);
				}
				Abilities.RemoveAt(i); // Remove the ability from the array
			}
		}
	}
}

ULE_AbilityObject* ULE_AbilityComponent::FindAbilityByClass(UPARAM(meta = (NotAllowNone)) TSubclassOf<ULE_AbilityObject> AbilityClass)
{
	if (AbilityClass)
	{
		for (ULE_AbilityObject* Ability : Abilities)
		{
			if (Ability->GetClass() == AbilityClass)
			{
				return Ability;
			}
		}
	}
	return nullptr;
}

void ULE_AbilityComponent::OnRep_Abilities()
{

}

void ULE_AbilityComponent::CallInputClient_Implementation(bool Pressed, ULE_AbilityObject* Ability, UObject* ObjectPayload, float FloatPayload, FTransform TransformPayload)
{
	if (Pressed)
	{
		Ability->OnInputPressed.Broadcast();
	}
	else
	{
		Ability->OnInputReleased.Broadcast();
	}
}
