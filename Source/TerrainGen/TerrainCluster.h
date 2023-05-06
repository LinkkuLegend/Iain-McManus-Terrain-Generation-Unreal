// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UtilsDataStructs.h"

#include "ActorPartition/PartitionActor.h"
#include "TerrainCluster.generated.h"

class UTerrainSection;
class ATerrain;

/**
 *
 */
UCLASS()
class TERRAINGEN_API ATerrainCluster : public AActor {
	GENERATED_BODY()

		

		ATerrainCluster();

	/** X offset from global components grid origin (in quads) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LandscapeComponent, meta = (AllowPrivateAccess = "true"))
		int32 ClusterBaseX;

	/** Y offset from global components grid origin (in quads) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LandscapeComponent, meta = (AllowPrivateAccess = "true"))
		int32 ClusterBaseY;


	MArray<float> ClusterHeightMap;

public:
	/** The array of LandscapeComponent that are used by the landscape */
		//TArray<UTerrainSection*> TerrainSections;

		/** The array of LandscapeComponent that are used by the landscape */
	UPROPERTY()
		TArray<TObjectPtr<UTerrainSection>> TerrainSections;

	ATerrain* Owner;

public:

	UFUNCTION()
		bool IsChunkLoaded(FInt32Vector2 chunk);

	UFUNCTION()
		void LoadChunk(FInt32Vector2 chunk, uint8 sectionsPerCluster);

	void LoadAllChunksInCluster();

	void HideOutOfRangeChunks(const FVector2D& SphereCenter, float SphereRadius);

	FORCEINLINE FInt32Vector2 GetClusterBase() const { return FInt32Vector2(ClusterBaseX, ClusterBaseY); }
	FORCEINLINE void SetClusterBase(FInt32Vector2 Base) { ClusterBaseX = Base.X; ClusterBaseY = Base.Y; }

	FORCEINLINE void SetClusterHeightMap(MArray<float> HeightMap) { ClusterHeightMap = HeightMap; }

	ATerrain* GetTerrain() const;

	void MakeMobilityStatic();

	void SafeDestroy();

	void AddChunkToQueue(TUniqueFunction<void()> ChunkLoad) const;
};
