// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include <TerrainGen/UtilsDataStructs.h>
#include "TerrainSection.generated.h"

class UProceduralMeshComponent;
/**
 * 
 */
UCLASS(Within = TerrainCluster)
class TERRAINGEN_API UTerrainSection : public USceneComponent {
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

	UPROPERTY()
		TArray <UProceduralMeshComponent*> ChunkMesh;

	UPROPERTY()
		TArray <UStaticMeshComponent*> ChunkStaticMesh;

	UPROPERTY()
		TArray<UStaticMesh*> StaticMesh;



public:
	UFUNCTION()
		bool IsSectorLoaded(FInt32Vector2 section);

	FORCEINLINE FInt32Vector2 GetSectionBase() const { return FInt32Vector2(SectionBaseX, SectionBaseY); }
	FORCEINLINE void SetSectionBase(FInt32Vector2 Base) { SectionBaseX = Base.X; SectionBaseY = Base.Y; }

	ATerrainCluster* GetCluster() const;

	void CreateSection(FInt32Vector2 SectorPos, FInt32Vector2 FirstChunkInSector, int32 numSubsections, int32 chunkSize, UTexture2D* heightMap);

	void CreateChunkMeshFromHeightMap(FInt32Vector2 ChunkPos,const MArray<float>& HeightMap);

	void GetChunkHeights(FInt32Vector2 ChunkPos, MArray<float>& HeightMap);

};
