// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "Engine/Texture2D.h"
#include "Math/NumericLimits.h"

#include "UtilsDataStructs.h"
#include "DA_ProcGenConfig.h"
#include "DA_BiomeConfig.h"
#include <TerrainGen/UtilsArray.h>

#include "ProcGenManager.generated.h"


UCLASS()
class TERRAINGEN_API AProcGenManager : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProcGenManager();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true))
	//TArray<FVector> Vertices;

	//UPROPERTY(EditAnywhere)
	//TArray<int> Triangles;


private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GenConfig", meta = (AllowPrivateAccess = "true"))
		UDA_ProcGenConfig* Config;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain", meta = (AllowPrivateAccess = "true"))
		class ATerrain* TargetTerrain;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain", meta = (AllowPrivateAccess = "true"))
		class UProceduralMeshComponent* TestTerrain;*/

	UFUNCTION(BlueprintCallable)
		void RegenerateTerrain();

	void PerformBiomeGeneration_LowRes(uint16 mapResolution);

	void PerformBiomeGeneration_HighRes(uint16 lowResMapResolution, uint32 targetResolution);

	UFUNCTION(BlueprintCallable)
		void Perform_HeightMapModification(int32 targetResolution);

	void PerformSpawnIndividualBiome(short BiomeIndex, uint16 HeightmapResolution);

	void InitializeNeighbourOffsets();

	bool IsTextureSafeToReadFrom(UTexture2D* Texture);

	void DebugBiomeToTexture(UTexture2D*& texture, MArray<uint8> pixels, uint16 resolution, uint8 NumBiomes, FString filename);

	uint8 CalculateHighResBiomeIndex(uint16 lowResMapSize, uint32 lowResX, uint32 lowResY, uint32 HighResX, uint32 HighResY);

	// Terrain Generation OBSOLETE
	/*void GenerateTile();*/


#pragma region "Mapgen Variables"

private:

	MArray <uint8> BiomeMap_LowRes;
	MArray <float> BiomeStrengths_LowRes;

	MArray <uint8> BiomeMap;
	MArray <float> BiomeStrengths;

	TArray<FInt32Vector2> NeighbourOffsets;

	//Debug purposes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* BiomeMapTextureLowRes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* BiomeMapTextureHighRes;

#pragma endregion

#pragma region "Texturing"

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texturing", meta = (AllowPrivateAccess = "true"))
		bool RegenerateLayers = true;

	void Perform_LayerSetup();

#pragma endregion

public:
	UFUNCTION(BlueprintImplementableEvent)
		void BiomeImageToDisk(UTexture2D* texture, const FString& filename);



};
