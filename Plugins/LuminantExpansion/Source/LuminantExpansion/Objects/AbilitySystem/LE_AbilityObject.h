// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "LE_ReplicatedObject.h"
#include "LE_AbilityObject.generated.h"

DECLARE_MULTICAST_DELEGATE(FInputPressed);
DECLARE_MULTICAST_DELEGATE(FInputReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopLineTrace);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopArcTrace);

class ULE_AbilityComponent;
class ULE_AbilityTask;
class UActivationDataObject;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMINANTEXPANSION_API ULE_AbilityObject : public ULE_ReplicatedObject
{
	GENERATED_BODY()

protected: //replicating the object

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	friend class ULE_AbilityComponent;

public: //Ability Propterties

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FString AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FString AbilityDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	UTexture2D* AbilityIcon = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float AbilityCooldown = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	FTimerHandle CooldownHandle;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Ability")
	float CooldownStartTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", Replicated)
	ULE_AbilityComponent* AbilityComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TArray<TObjectPtr<ULE_AbilityTask>> AbilityTasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bIsActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bIsOnCooldown = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TArray<UActivationDataObject*> ActivationData;

	//Tag stuff

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer GiveTagsOnActivated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer GiveTagsOnDeactivated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer RemoveTagsOnActivated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer RemoveTagsOnDeactivated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer RequiredToActivateTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer CannotActivateWithTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer ActivationTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer DeactivationTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tags")
	FGameplayTagContainer FailAbilityTags;

	//Delegates and timers

	FInputPressed OnInputPressed;
	FInputReleased OnInputReleased;


	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability")
	FStopLineTrace StopLineTrace;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability")
	FStopArcTrace StopArcTrace;

public:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInitializeAbility();
	UFUNCTION(Client, Reliable)
	void ClientInitializeAbility(ULE_AbilityComponent* AC);
	UFUNCTION(Server, Reliable, WithValidation)
	void HandleActivatorTriggered();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ability")
	void BeginAbility(UObject* ObjectPayload, float floatPayload, FTransform TransformPayload);

	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Ability", meta = (AdvancedDisplay = "ObjectPayload, FloatPayload, TransformPayload"))
	void ActivateAbility(UObject* ObjectPayload, float FloatPayload, FTransform TransformPayload);

	UFUNCTION(BlueprintCallable, Category = "Ability", Server, Reliable, WithValidation)
	void DeactivateAbility(bool bFailed = false);

	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Ability")
	void OnEndAbility(bool bFailed = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Ability")
	void CommitAbility();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void OnCooldownFinished();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AActor* GetOwnerActor() const;

};
