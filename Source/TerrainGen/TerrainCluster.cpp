// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainCluster.h"
#include "TerrainSection.h"
#include "Terrain.h"

void ATerrainCluster::LoadChunk(FInt32Vector2 chunk, uint8 sectionsPerCluster) {
	FInt32Vector2 SectorPosition, ClusterPosition;
	//ATerrain::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);
	GetTerrain()->ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	for(int x = 0; x < sectionsPerCluster; x++) {
		for(int y = 0; y < sectionsPerCluster; y++) {
			//UE_LOG(LogTemp, Warning, TEXT("Have to create Section: %dx%d"), SectorPosition.X + x, SectorPosition.Y + y);
			FString SectorID = "Sector ";
			SectorID.Append(FString::FromInt(SectorPosition.X + x));
			SectorID.Append("-");
			SectorID.Append(FString::FromInt(SectorPosition.Y + y));

			//UTerrainSection* Section = CreateDefaultSubobject<UTerrainSection>(FName(SectorID));
			UTerrainSection* TerrainSection = NewObject<UTerrainSection>(this, FName(SectorID));
			TerrainSection->CreateSection(SectorPosition.X + x, SectorPosition.Y + y, sectionsPerCluster, 64, nullptr);
			TerrainSections.Add(TerrainSection);
		}
	}
}

bool ATerrainCluster::IsChunkLoaded(FInt32Vector2 chunk) {
	for(UTerrainSection* Component : TerrainSections)
		if(Component->IsChunkLoaded(chunk))
			return true;
	return false;
}

ATerrain* ATerrainCluster::GetTerrain() const {
	return CastChecked<ATerrain>(GetOuter());
}