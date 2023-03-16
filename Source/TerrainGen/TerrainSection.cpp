// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainSection.h"

void UTerrainSection::CreateSection(int32 posX, int32 posY, int32 numSubsections, int32 chunkSize, UTexture2D* heightMap) {
	SectionBaseX = posX;


	SectionBaseY = posY;
	NumChunks = numSubsections;
	ChunkNumQuads = chunkSize - 1;
	TotalNumQuads = ChunkNumQuads * (numSubsections ^ 2);
	//UE_LOG(LogTemp, Warning, TEXT("Created Section: %dx%d"), SectionBaseX, SectionBaseY);
}

bool UTerrainSection::IsChunkLoaded(FInt32Vector2 chunk) {
	if(SectionBaseX <= chunk.X && chunk.X <= SectionBaseX + (NumChunks - 1))
		if(SectionBaseY <= chunk.Y && chunk.Y <= SectionBaseY + (NumChunks - 1))
			return true;

	return false;
}