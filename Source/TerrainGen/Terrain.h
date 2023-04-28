// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <TerrainGen/UtilsDataStructs.h>
#include "Runtime/Core/Public/Containers/Queue.h"
#include "Terrain.generated.h"

class ATerrainCluster;
class UTerrainSection;


USTRUCT()
struct FTerrainInfo {

	GENERATED_BODY()

public:

	/* In one of the axis */
	uint8 static const SectionsPerCluster = 2;
	/* In one of the axis */
	uint8 static const ChunksPerSection = 2;
	/* In num of vertex in an axis */
	uint8 static const ChunkSize = 64;
	/* This is in UE5 Units */
	uint8 static const QuadSize = 100;


	static void ChunkToSectionAndCluster(FInt32Vector2 chunk, FInt32Vector2& sector, FInt32Vector2& cluster) {
		int sectionX, sectionY;
		int clusterX, clusterY;

		float sectionFactor = ChunksPerSection * 1.0f;
		float clusterFactor = ChunksPerSection * SectionsPerCluster * 1.0f;


		sectionX = FMath::FloorToInt(chunk.X / sectionFactor);
		sectionY = FMath::FloorToInt(chunk.Y / sectionFactor);

		clusterX = FMath::FloorToInt(chunk.X / clusterFactor);
		clusterY = FMath::FloorToInt(chunk.Y / clusterFactor);


		sector = FInt32Vector2(sectionX, sectionY);
		cluster = FInt32Vector2(clusterX, clusterY);
	}

	/*
	*  Take into account we will return the first chunk
	*/
	static void SectionToChunkAndCluster(FInt32Vector2 sector, FInt32Vector2& chunk, FInt32Vector2& cluster) {
		int chunkX, chunkY;
		int clusterX, clusterY;

		float chunkFactor = ChunksPerSection * 1.0f;
		float clusterFactor = SectionsPerCluster * 1.0f;


		chunkX = FMath::FloorToInt(sector.X * chunkFactor);
		clusterX = FMath::FloorToInt(sector.X / clusterFactor);

		chunkY = FMath::FloorToInt(sector.Y * chunkFactor);
		clusterY = FMath::FloorToInt(sector.Y / clusterFactor);

		chunk = FInt32Vector2(chunkX, chunkY);
		cluster = FInt32Vector2(clusterX, clusterY);
	}

	/*
	*  Take into account we will return the first chunk and sector
	*/
	static void ClusterToChunkAndSection(FInt32Vector2 cluster, FInt32Vector2& chunk, FInt32Vector2& section) {
		int chunkX, chunkY;
		int sectionX, sectionY;

		float chunkFactor = ChunksPerSection * SectionsPerCluster * 1.0f;
		float sectorFactor = SectionsPerCluster * 1.0f;

		chunkX = FMath::FloorToInt(cluster.X * chunkFactor);
		sectionX = FMath::FloorToInt(cluster.X / sectorFactor);

		chunkY = FMath::FloorToInt(cluster.Y * chunkFactor);
		sectionY = FMath::FloorToInt(cluster.Y / sectorFactor);

		chunk = FInt32Vector2(chunkX, chunkY);
		section = FInt32Vector2(sectionX, sectionY);
	}


	/*									______
	*  If ChunksPerSection = 2 we		|2	3|
	*  will get the next positions		|0	1|
	*/
	static int ChunkToSectionPosition(FInt32Vector2 chunk) {
		int X = chunk.X % ChunksPerSection;
		int Y = chunk.Y % ChunksPerSection;

		return X + ChunksPerSection * Y;
	}


};

UCLASS()
class TERRAINGEN_API ATerrain : public AActor {
	GENERATED_BODY()

private:
	ATerrain();


private:

	/* The number of Chunks to load in any direccion, the chunk where the player is located is included */
	uint8 VisionRange;

	//FInt32Vector2 CurrentProxy(FVector Location);


private:

	UPROPERTY(Transient)
		TArray<ATerrainCluster*> TerrainClusters;

	UTexture2D* DebugHeightmapTexture;

	bool IsClusterLoaded(FInt32Vector2 cluster);

	void Initialize();

	void DebugPrintLoadedChunks();

	void Reset();

public:

	void GetHeights(int ChunkPosX, int ChunkPosY, int NumChunksX, int NumChunksY, MArray<float>& HeightMap);
	void SetHeights(int ChunkPosX, int ChunkPosY, const MArray<float>& HeightMap);
	void SetTextures(int ChunkPosX, int ChunkPosY, const MArray<uint8>& BiomeMap);

	int GetVisionRadius();

	UFUNCTION(BlueprintCallable)
		void UpdateTerrain(FVector PlayerPosition);

	UFUNCTION()
		bool IsChunkLoaded(FInt32Vector2 chunk);

	/*UFUNCTION()
		void LoadChunk(FInt32Vector2 chunk);*/

	UFUNCTION()
		void LoadClusterAsync(FInt32Vector2 cluster);

	UFUNCTION(BlueprintCallable)
		FORCEINLINE ATerrain* GetTerrain() { return this; };

	

private:

	ATerrainCluster* GetClusterByChunk(FInt32Vector2 chunk);

	UTerrainSection* GetSectionByChunk(FInt32Vector2 chunk);

	UFUNCTION()
		void HideOutOfRangeChunks(const FVector2D& PlayerLocation, float VisionRangeRadius);

	void PopulateArrayInSpiralOrder(FInt32Vector2 PointSW, FInt32Vector2 PointNE, TArray<FInt32Vector2>& SpiralGrid);

};
