// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

class ATerrainCluster;

UCLASS()
class TERRAINGEN_API ATerrain : public AActor {
	GENERATED_BODY()

private:
	ATerrain();

private:

	/* In number of Sections in one of the axis */
	uint8 VisionRange;
	/* In one of the axis */
	uint8 static const SectionsPerCluster = 2;
	/* In one of the axis */
	uint8 static const ChunksPerSection = 2;
	/* In num of vertex in an axis */
	uint8 static const ChunkSize = 64;
	/* This is in UE5 Units */
	uint8 static const QuadSize = 100;


	FInt32Vector2 CurrentProxy(FVector Location);


private:

	UPROPERTY(Transient)
		TArray<ATerrainCluster*> TerrainClusters;

	UTexture2D* DebugHeightmapTexture;

	bool IsClusterLoaded(FInt32Vector2 cluster);

	void Initialize();

	void DebugPrintLoadedChunks();

	void Reset();

public:

	void static ChunkToSectionAndCluster(FInt32Vector2 chunk, FInt32Vector2& sector, FInt32Vector2& cluster);

	UFUNCTION(BlueprintCallable)
		void UpdateTerrain(FVector PlayerPosition);

	UFUNCTION()
		bool IsChunkLoaded(FInt32Vector2 chunk);

	UFUNCTION()
		void LoadChunk(FInt32Vector2 chunk);

	UFUNCTION(BlueprintCallable)
		FORCEINLINE ATerrain* GetTerrain() { return this; }


};
