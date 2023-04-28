// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilsDataStructs.h"


struct TERRAINGEN_API FTerrainGenCurves {
public:
	UCurveFloat* ContinentalnessCurve;
};

/**
 * This class takes cares of functions related to generate specialized perlin noise for the terrain
 */
static class TERRAINGEN_API WorldTerrainGen {

	

public:
	//WorldTerrainGen();
	//~WorldTerrainGen();

	static inline FTerrainGenCurves PerlinNoiseGenCurves = FTerrainGenCurves();

	// Debug
	UTexture2D* GenerateClusterTexture(FIntPoint Cluster, const UCurveFloat* const Curve, int32 Octaves = 1, float Persistence = 0.5f, float Frequency = 2.0f);

	static MArray <float> GetClusterHeights(FIntPoint Cluster);

	static void InitializeCurves(FTerrainGenCurves Curves);

private:

	static MArray <float> PerlinTerrainGen(FIntPoint Cluster, int32 Octaves = 1, float Persistence = 0.5f, float Frequency = 2.0f);

	static MArray<float> ApplyCurveToPerlin(MArray<float> PerlinNoise, const UCurveFloat* const Curve);
};
