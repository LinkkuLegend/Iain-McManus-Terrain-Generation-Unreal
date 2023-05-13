// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilsDataStructs.h"


struct TERRAINGEN_API FTerrainGenCurves {
public:
	UCurveFloat* ContinentalnessCurve;
	UCurveFloat* ErosionCurve;
};

/**
 * This class takes cares of functions related to generate specialized perlin noise for the terrain
 */
static class TERRAINGEN_API WorldTerrainGen {

public:
	//WorldTerrainGen();
	//~WorldTerrainGen();

	static inline FTerrainGenCurves PerlinNoiseGenCurves = FTerrainGenCurves();

	static inline int32 Seed = 0;

	// Debug
	static UTexture2D* GenerateClusterTexture(MArray<float> HeightMap, const UCurveFloat* const Curve);

	static MArray<float> GenerateClusterHeightMap(FIntPoint StartCluster, FIntPoint EndCluster, uint8 Maptype, float BaseFrequency, const UCurveFloat* const Curve, int32 Octaves = 1, float Persistence = 0.5f, float Frequency = 2.0f);

	static UTexture2D* MixMainTextures(MArray<float> ContinentalnessHeightMap, MArray<float> ErosionHeightMap, const UCurveFloat* const Curve);

	static MArray <float> GetClusterHeights(FIntPoint Cluster);

	static void Initialize(FTerrainGenCurves Curves, int32 SeedLocal);

	static void PerlinReset(); //Put it back to private later

private:

	static MArray <float> PerlinTerrainGen(FIntPoint Cluster, uint8 MapType, float BaseFrequency = 1/64.f, int32 Octaves = 1, float Persistence = 0.5f, float Frequency = 2.0f);

	static MArray<float> ApplyCurveToPerlin(MArray<float> PerlinNoise, const UCurveFloat* const Curve);

	static float PerlinNoise2D(const FVector2D& Location, uint8 Permute);

	static MArray<float> MixMainHeightMaps(MArray<float> ContinentalnessHeightMap, MArray<float> ErosionHeightMap);
	
};
