// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LE_AbilityObject.h"
#include "GameplayTagContainer.h"
#include "LE_AbilityComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTagChanged, FGameplayTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayTagChanged, FGameplayTag, Tag);

class UEnhancedInputComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class LUMINANTEXPANSION_API ULE_AbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULE_AbilityComponent();

	friend class ULE_AbilityObject;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", ReplicatedUsing = OnRep_Abilities)
	TArray<TObjectPtr<ULE_AbilityObject>> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	UEnhancedInputComponent* EnhancedInputComponent;

	UFUNCTION()
	void OnRep_Abilities();

	//Gameplay tag container

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayTagContainer AbilityComponentTags;

	FTagChanged OnTagAdded;
	FTagChanged OnTagRemoved;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability")
	FGameplayTagChanged OnGameplayTagAdded;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability")
	FGameplayTagChanged OnGameplayTagRemoved;

	void AbilityTagActivation(FGameplayTag Tag)
	{
		for (ULE_AbilityObject* Ability : Abilities)
		{
			if (Ability && Ability->ActivationTags.IsValid())
			{
				if (Ability->ActivationTags.HasTag(Tag))
				{
					Ability->BeginAbility(nullptr, 0.f, FTransform::Identity);
				}
			}
			if (Ability && Ability->DeactivationTags.IsValid())
			{
				if (Ability->DeactivationTags.HasTag(Tag))
				{
					Ability->DeactivateAbility(false);
				}
			}
			if (Ability && Ability->FailAbilityTags.IsValid())
			{
				if (Ability->FailAbilityTags.HasTag(Tag))
				{
					Ability->DeactivateAbility(true);
				}
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void AddTag(FGameplayTag Tag)
	{
		AbilityComponentTags.AddTag(Tag);
		OnTagAdded.Broadcast(Tag);
		OnGameplayTagAdded.Broadcast(Tag);
		AbilityTagActivation(Tag);
	}

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void RemoveTag(FGameplayTag Tag)
	{
		AbilityComponentTags.RemoveTag(Tag);
		OnTagRemoved.Broadcast(Tag);
		OnGameplayTagRemoved.Broadcast(Tag);
	}

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void AppendTags(FGameplayTagContainer Tags)
	{
		for (const FGameplayTag& Tag : Tags)
		{
			AddTag(Tag);
			OnTagAdded.Broadcast(Tag);
			OnGameplayTagAdded.Broadcast(Tag);
			AbilityTagActivation(Tag);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void RemoveTags(FGameplayTagContainer Tags)
	{
		for (const FGameplayTag& Tag : Tags)
		{
			RemoveTag(Tag);
			OnTagRemoved.Broadcast(Tag);
			OnGameplayTagRemoved.Broadcast(Tag);
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ability")
	ULE_AbilityObject* GrantAbility(TSubclassOf<ULE_AbilityObject> AbilityClass);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ability")
	void RemoveAbility(TSubclassOf<ULE_AbilityObject> AbilityToRemove);

	UFUNCTION(Client, Reliable)
	void CallInputClient(bool Pressed, ULE_AbilityObject* Ability, UObject* ObjectPayload, float FloatPayload, FTransform TransformPayload);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = AbilityClass), Category = "Ability")
	ULE_AbilityObject* FindAbilityByClass(UPARAM(meta = (NotAllowNone)) TSubclassOf<ULE_AbilityObject> AbilityClass);
	
};
