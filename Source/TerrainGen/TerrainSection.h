// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "Runtime/Core/Public/Containers/Queue.h"

#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshOperations.h"
#include "StaticMeshAttributes.h"
#include <Runtime/MeshConversion/Public/MeshDescriptionBuilder.h>
#include "RealtimeMeshLibrary.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshComponent.h"

#include "Components/PrimitiveComponent.h"
#include <TerrainGen/UtilsDataStructs.h>
#include "TerrainSection.generated.h"


USTRUCT(BlueprintType)
struct TERRAINGEN_API FChunkData {
	GENERATED_BODY()
public:
	FInt32Vector2 ChunkPos;
	URealtimeMeshSimple* Mesh;
	URealtimeMeshComponent* ChunkComponent;
};

//USTRUCT(BlueprintType)
//struct TERRAINGEN_API FChunkDataTempAsync {
//	GENERATED_BODY()
//public:
//	FInt32Vector2 ChunkPos;
//	FMeshDescription* MeshDescription;
//};


//USTRUCT(BlueprintType)
//struct TERRAINGEN_API FBiomeConfig {
//
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//		UDA_BiomeConfig* Biome;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
//		float Weighting = 1.f;
//
//};


/**
 *
 */
UCLASS(Within = TerrainCluster)
class TERRAINGEN_API UTerrainSection : public UObject {
	GENERATED_BODY()

		/** X offset from global components grid origin (in quads) */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LandscapeComponent, meta = (AllowPrivateAccess = "true"))
		int32 SectionBaseX;

	/** Y offset from global components grid origin (in quads) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LandscapeComponent, meta = (AllowPrivateAccess = "true"))
		int32 SectionBaseY;

	/** Total number of quads for this section, has to be >0 */
	UPROPERTY()
		int32 TotalNumQuads;

	/** Number of quads for a chunk of the section. ChunkNumQuads+1 must be a power of two. */
	UPROPERTY()
		int32 ChunkNumQuads;

	/** Number of chunks in X or Y axis */
	UPROPERTY()
		int32 NumChunks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LandscapeComponent, meta = (AllowPrivateAccess = "true"))
		UMaterialInterface* OverrideMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LandscapeComponent, AdvancedDisplay, meta = (AllowPrivateAccess = "true"))
		UMaterialInterface* OverrideHoleMaterial;

	UPROPERTY()
		UTexture2D* HeightmapTexture;

	//UPROPERTY()
	//	TArray <UProceduralMeshComponent*> ChunkMesh;

	//UPROPERTY()
	//	TArray <UStaticMeshComponent*> ChunkStaticMesh;

	UPROPERTY()
		TArray<FChunkData> StaticMesh;


	TQueue<FChunkData> StaticMeshLoadAsync;
	//TQueue<TUniqueFunction<void()>> StaticMeshLoadAsync;

public:
	UFUNCTION()
		bool IsSectorLoaded(FInt32Vector2 section);

	FORCEINLINE FInt32Vector2 GetSectionBase() const { return FInt32Vector2(SectionBaseX, SectionBaseY); }
	FORCEINLINE void SetSectionBase(FInt32Vector2 Base) { SectionBaseX = Base.X; SectionBaseY = Base.Y; }

	ATerrainCluster* GetCluster() const;
	//void AddChunkToQueue(TUniqueFunction<void()> ChunkLoad) const;

	//DEPRECATED
	//void CreateSection(FInt32Vector2 SectorPos, FInt32Vector2 FirstChunkInSector, int32 numSubsections, int32 chunkSize, UTexture2D* heightMap);
	void LoadSection(FInt32Vector2 SectionPos, const MArray<float>& HeightMap);

	//void CreateChunkMeshFromHeightMap(FInt32Vector2 ChunkPos, const MArray<float>& HeightMap);

	void LoadChunkAsync(FInt32Vector2 ChunkSubsection, const MArray<float>& HeightMap, UTerrainSection* Section, URealtimeMeshSimple* MeshPointerToLoad);

	void CreateMaterialsFromBiomeMap(FInt32Vector2 ChunkPos, const MArray<uint8>& BiomeMap);

	//void GetChunkHeights(FInt32Vector2 ChunkPos, MArray<float>& HeightMap);

	void HideOutOfRangeChunks(const FVector2D& SphereCenter, float SphereRadius);

	void SafeDestroy();

private:

	FCriticalSection ChunkLoadLock;

	void LoadDataFromAsyncLoad();

};
