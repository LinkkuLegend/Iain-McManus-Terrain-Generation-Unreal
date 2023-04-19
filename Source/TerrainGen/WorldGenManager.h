// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Terrain.h"
#include "WorldGenManager.generated.h"

UCLASS()
class TERRAINGEN_API AWorldGenManager : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldGenManager();

private:

	// Reference to the player, mainly to retrieve his movement for terrain updates
	UPROPERTY()
		ACharacter* PlayerCharacter;

	// The controller of the physical terrain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain", meta = (AllowPrivateAccess = "true"))
		class ATerrain* TargetTerrain;

	float LastTimeExecuted;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
