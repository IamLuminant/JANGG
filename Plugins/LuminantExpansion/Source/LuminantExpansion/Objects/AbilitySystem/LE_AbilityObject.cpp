// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_AbilityObject.h"
#include "LE_AbilityComponent.h"
#include "ActivationDataObject.h"
#include "EnhancedInputComponent.h"
#include "LE_WaitTimer.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameStateBase.h"

void ULE_AbilityObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULE_AbilityObject, AbilityComponent);
	DOREPLIFETIME(ULE_AbilityObject, AbilityCooldown);
	DOREPLIFETIME(ULE_AbilityObject, CooldownStartTime);
}

AActor* ULE_AbilityObject::GetOwnerActor() const
{
	if (AbilityComponent)
	{
		return AbilityComponent->GetOwner();
	}
	return nullptr;
}

void ULE_AbilityObject::ServerInitializeAbility_Implementation()
{
	ClientInitializeAbility(AbilityComponent);
}
bool ULE_AbilityObject::ServerInitializeAbility_Validate()
{
	return true;
}

void ULE_AbilityObject::ClientInitializeAbility_Implementation(ULE_AbilityComponent* AC)
{
	AbilityComponent = AC;
	if (!AbilityComponent || !AbilityComponent->GetOwner()) return;
	
	if (UEnhancedInputComponent* EInputComp = AbilityComponent->EnhancedInputComponent)
	{
		for (UActivationDataObject* Activator : ActivationData)
		{
			if (Activator)
			{
				// 1. Initialize hardware binding
				Activator->InitializeActivation(EInputComp);
				// 2. Bind the dynamic delegate to our local UFUNCTION proxy
				Activator->OnActivationEvent.AddUniqueDynamic(this, &ULE_AbilityObject::HandleActivatorTriggered);
			}
		}
	}
}

void ULE_AbilityObject::HandleActivatorTriggered_Implementation()
{ 
	if (!bIsActivated && !bIsOnCooldown)
	{
		BeginAbility(nullptr, 0.0f, FTransform());
	}
}

bool ULE_AbilityObject::HandleActivatorTriggered_Validate()
{
	return true;
}

void ULE_AbilityObject::BeginAbility(UObject* ObjectPayload, float FloatPayload, FTransform TransformPayload)
{
	if (AbilityComponent)
	{
		if (!AbilityComponent->AbilityComponentTags.HasAll(RequiredToActivateTags))
		{
			return;
		}
		if (AbilityComponent->AbilityComponentTags.HasAny(CannotActivateWithTags))
		{
			return;
		}
		if (!bIsActivated && !bIsOnCooldown)
		{
			bIsActivated = true;
			AbilityComponent->AppendTags(GiveTagsOnActivated);
			AbilityComponent->RemoveTags(RemoveTagsOnActivated);
			ActivateAbility(ObjectPayload, FloatPayload, TransformPayload);
		}
	}
}

void ULE_AbilityObject::DeactivateAbility_Implementation(bool bFailed = false)
{
	if (bIsActivated)
	{
		bIsActivated = false;
		AbilityComponent->AppendTags(GiveTagsOnDeactivated);
		AbilityComponent->RemoveTags(RemoveTagsOnDeactivated);
		OnEndAbility(bFailed);
		TArray<TObjectPtr<ULE_AbilityTask>> TasksToDeactivate = AbilityTasks; // copy
		for (ULE_AbilityTask* Task : TasksToDeactivate)
		{
			if (Task)
			{
				Task->StopTask();
			}
		}
		AbilityTasks.Empty();
	}
}
bool ULE_AbilityObject::DeactivateAbility_Validate(bool bFailed = false) { return true; }

void ULE_AbilityObject::CommitAbility_Implementation()
{
	if (AbilityCooldown > 0.0f)
	{
		bIsOnCooldown = true;
		GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ULE_AbilityObject::OnCooldownFinished, AbilityCooldown, false);
		if (AGameStateBase* GameState = GetWorld()->GetGameState())
		{
			// Returns the synced server world time in seconds
			CooldownStartTime = GameState->GetServerWorldTimeSeconds();
		}
	}
}

void ULE_AbilityObject::OnCooldownFinished()
{
	bIsOnCooldown = false;
	CooldownHandle.Invalidate();
}

void ULE_AbilityObject::ActivateAbility_Implementation(UObject* ObjectPayload, float FloatPayload, FTransform TransformPayload)
{
	
}

void ULE_AbilityObject::OnEndAbility_Implementation(bool bFailed = false)
{
	// Default implementation does nothing. Override in derived classes.
}

