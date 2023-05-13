// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Terrain.h"
#include <TerrainGen/WorldTerrainGen.h>
#include "WorldGenManager.generated.h"

UENUM()
enum class MapType : uint8 {
	Continentalness = 0,
	Erosion = 1,
	PeaksValleys = 2,
	Temperature = 3,
	Humidity = 4
};

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

	//WorldTerrainGen TerrainGen; //This is now a static class



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


#pragma region "Perlin noise Editor Debug"
private:

	float BaseFrequencyContinentalness = 1 / 4096.f;
	float BaseFrequencyErosion = 1 / 4096.f;
	float OctavesContinentalness = 7;
	float OctavesErosion = 7;

	// Editor Function
	UFUNCTION(BlueprintCallable, Category = "Perlin Debug")
		void GenerateHeightMapByClusterEditor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		FIntPoint DebugStartCluster;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		FIntPoint DebugEndCluster;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		int32 Seed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* WorldHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* Continentalness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* ContinentalnessCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* Erosion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* ErosionCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* PeaksAndValleys;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* Temperature;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perlin Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* Humidity;

#pragma endregion

	FORCEINLINE UCurveFloat* GetContinentalnessCurve() const {
		return ContinentalnessCurve;
	}
};
