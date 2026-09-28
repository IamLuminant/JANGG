// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LE_InteractionInterface.h"
#include "InteractableActorCPP.generated.h"

class UMaterialInstance;
class UStaticMesh;
class UStaticMeshComponent;
class ALE_InteractionActor;
class UChildActorComponent;

UCLASS()
class JANGG_API AInteractableActorCPP : public AActor, public ILE_InteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActorCPP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;// Called when properties are updated in the editor
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* OverlayMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	UMaterialInstance* MeshColliderMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	bool bShowColliderMesh = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	UChildActorComponent* InteractionActor = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InterfaceInteract_Implementation(ACharacter* Interactor) override;

};
