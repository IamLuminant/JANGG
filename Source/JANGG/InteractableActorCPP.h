// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActorCPP.generated.h"

class MaterialInstance;

UCLASS()
class JANGG_API AInteractableActorCPP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActorCPP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	MaterialInstance* OverlayMaterial = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
