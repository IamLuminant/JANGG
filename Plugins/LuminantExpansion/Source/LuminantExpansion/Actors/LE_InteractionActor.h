// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "LE_InteractionActor.generated.h"

UCLASS()
class LUMINANTEXPANSION_API ALE_InteractionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALE_InteractionActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ColliderShape;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMesh* ColliderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	bool ShowCollider = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UMaterialInterface* ColliderMaterial;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ToggleFocus(bool F);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when properties are updated in the editor
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
	bool Focused = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
	float InteractionDuration = 0.0f;

};
