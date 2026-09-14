// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LE_AbilityTask.h"
#include "GameplayTagContainer.h"
#include "LE_WaitTagsRemovedTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTagsRemoved);


UCLASS()
class LUMINANTEXPANSION_API ULE_WaitTagsRemovedTask : public ULE_AbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Tasks", meta = (DefaultToSelf = "AbilityObject", AdvancedDisplay = "AbilityObject", BlueprintInternalUseOnly = "TRUE"))
	static ULE_WaitTagsRemovedTask* WaitTagsRemoved(
		UPARAM(meta = (NotAllowNone)) ULE_AbilityObject* AbilityObject = nullptr,
		FGameplayTagContainer TagsToWaitFor = FGameplayTagContainer()
	);

protected:

	virtual void Activate() override;

private:

	UPROPERTY()
	FGameplayTagContainer TagsToWaitFor;

	UPROPERTY()
	FGameplayTagContainer CachedAbilityTags;

	UPROPERTY(BlueprintAssignable)
	FTagsRemoved OnTagsRemoved;

	UFUNCTION()
	void OnTagAdded(FGameplayTag AddedTag);
	
};
