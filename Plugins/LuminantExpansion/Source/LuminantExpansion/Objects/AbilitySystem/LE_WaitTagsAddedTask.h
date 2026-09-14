// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LE_AbilityTask.h"
#include "GameplayTagContainer.h"
#include "LE_WaitTagsAddedTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTagsAdded);

UCLASS()
class LUMINANTEXPANSION_API ULE_WaitTagsAddedTask : public ULE_AbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Tasks", meta = (DefaultToSelf = "AbilityObject", AdvancedDisplay = "AbilityObject", BlueprintInternalUseOnly = "TRUE"))
	static ULE_WaitTagsAddedTask* WaitTagsAdded(
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
	FTagsAdded OnTagsAdded;

	UFUNCTION()
	void OnTagAdded(FGameplayTag AddedTag);
	
};
