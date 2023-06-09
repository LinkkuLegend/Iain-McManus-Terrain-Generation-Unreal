// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilsDataStructs.h"

struct TERRAINGEN_API HeightMapVariables {
public:
	float MaxValue;
	float MinValue;
	float Mean;
	float StandardDeviation;
};

/**
 *
 */
static class TERRAINGEN_API TerrainAlgorithms {
public:

	// This is for the actual calculation of the flood, but we are gonna use another way
	static MArray<float> CalculateWaterAreas(const MArray <float>& HeightMap);

	// The one used to calculate the flood
	static MArray<float> CalculateWaterAreasSimplified(const MArray <float>& HeightMap);
	static TSet<FIntPoint> CalculateRiverShape(const MArray<float>& ShapeMap, FIntPoint StartPoint);
	static TArray<FIntPoint> BorderPointsInOrder(const MArray<int8>& ShapeMap, FIntPoint StartPoint, TArray<FIntPoint> PreviousPoint);

private:

	static TArray<FIntPoint> GetBorderFromFloodMap(const MArray<float>& FloodMap);
	static TArray<FIntPoint> GetBorderSection(const MArray<int8>& ShapeMap, FIntPoint StartPoint, FIntPoint PreviousPoint);
	static TArray<FIntPoint> GetBorderFlood(const MArray<float>& FloodMap, FIntPoint StartPoint);
	//static bool ExploreBorder(onst MArray<float>& FloodMap, FIntPoint StartPoint, int8 direction);

	// This is for the actual calculation of the flood, but we are gonna use another way
	static void CalculateVariables(const MArray<float>& HeightMap, HeightMapVariables& Variables);
	static void Flood(const MArray<float>& HeightMap, MArray<float>& FloodMap, MArray<bool>& CheckedAreas, FIntPoint FloodStart, float FloodThreshold);
	static TArray<FIntPoint> GetSurroundingHeights(const MArray<float>& HeightMap, FIntPoint BasePoint);
};
