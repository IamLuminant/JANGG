// Copyright 2025, Luminant. All Rights Reserved.


#include "LE_WaitTagsRemovedTask.h"
#include "LE_AbilityObject.h"
#include "LE_AbilityComponent.h"
#include "Engine/Engine.h"

ULE_WaitTagsRemovedTask* ULE_WaitTagsRemovedTask::WaitTagsRemoved(
	UPARAM(meta = (NotAllowNone)) ULE_AbilityObject* AbilityObject,
	FGameplayTagContainer TagsToWaitFor
)
{
	if (!AbilityObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create WaitTagsRemovedTask: Invalid CallingObject!"));
		return nullptr;
	}
	ULE_WaitTagsRemovedTask* NewTask = NewObject<ULE_WaitTagsRemovedTask>(AbilityObject);
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

void ULE_WaitTagsRemovedTask::Activate()
{
	Super::Activate();
	if (OwningAbility)
	{
		DelegateHandle = OwningAbility->AbilityComponent->OnTagAdded.AddUObject(this, &ULE_WaitTagsRemovedTask::OnTagAdded);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to activate"));
	}
}

void ULE_WaitTagsRemovedTask::OnTagAdded(FGameplayTag AddedTag)
{
	if (TagsToWaitFor.HasTag(AddedTag))
	{
		CachedAbilityTags.AddTag(AddedTag);
		if (OwningAbility && OwningAbility->AbilityComponent)
		{
			if (!OwningAbility->AbilityComponent->AbilityComponentTags.HasAnyExact(CachedAbilityTags))
			{
				OnTagsRemoved.Broadcast();
				StopTask();
			}
		}
	}
}

