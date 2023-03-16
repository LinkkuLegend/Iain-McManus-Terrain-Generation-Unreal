// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "TerrainSection.generated.h"

/**
 * 
 */
UCLASS(Within = TerrainCluster)
class TERRAINGEN_API UTerrainSection : public UPrimitiveComponent {
	GENERATED_BODY()

		//UTerrainSection();

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

public:
	UFUNCTION()
		bool IsChunkLoaded(FInt32Vector2 chunk);

	FORCEINLINE FInt32Vector2 GetSectionBase() const { return FInt32Vector2(SectionBaseX, SectionBaseY); }
	FORCEINLINE void SetSectionBase(FInt32Vector2 Base) { SectionBaseX = Base.X; SectionBaseY = Base.Y; }

	void CreateSection(int32 posX, int32 posY, int32 numSubsections, int32 chunkSize, UTexture2D* heightMap);
};
