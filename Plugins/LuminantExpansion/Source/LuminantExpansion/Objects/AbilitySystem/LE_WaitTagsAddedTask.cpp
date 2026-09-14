// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_WaitTagsAddedTask.h"
#include "LE_AbilityObject.h"
#include "LE_AbilityComponent.h"
#include "Engine/Engine.h"

ULE_WaitTagsAddedTask* ULE_WaitTagsAddedTask::WaitTagsAdded(
	UPARAM(meta = (NotAllowNone)) ULE_AbilityObject* AbilityObject,
	FGameplayTagContainer TagsToWaitFor
)
{
	if (!AbilityObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create WaitTagsAddedTask: Invalid CallingObject!"));
		return nullptr;
	}
	ULE_WaitTagsAddedTask* NewTask = NewObject<ULE_WaitTagsAddedTask>(AbilityObject);
	NewTask->OwningAbility = NewTask->GetTypedOuter<ULE_AbilityObject>();
	if (NewTask->OwningAbility)
	{
		NewTask->OwningAbility->AbilityTasks.Add(NewTask);
		NewTask->TagsToWaitFor = TagsToWaitFor;
	}
	else
	{
		NewTask->ConditionalBeginDestroy();
		NewTask = nullptr;
	}
	return NewTask;
}

void ULE_WaitTagsAddedTask::Activate()
{
	Super::Activate();
	if (OwningAbility)
	{
		DelegateHandle = OwningAbility->AbilityComponent->OnTagAdded.AddUObject(this, &ULE_WaitTagsAddedTask::OnTagAdded);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to activate"));
	}
}

void ULE_WaitTagsAddedTask::OnTagAdded(FGameplayTag AddedTag)
{
	if (TagsToWaitFor.HasTag(AddedTag))
	{
		CachedAbilityTags.AddTag(AddedTag);
		if (OwningAbility && OwningAbility->AbilityComponent)
		{
			if (OwningAbility->AbilityComponent->AbilityComponentTags.HasAllExact(CachedAbilityTags))
			{
				OnTagsAdded.Broadcast();
				StopTask();
			}
		}
	}
}

