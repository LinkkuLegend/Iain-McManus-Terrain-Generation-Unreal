// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainCluster.h"
#include "TerrainSection.h"
#include "Terrain.h"

ATerrainCluster::ATerrainCluster() {
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent0"));
	RootComponent = SceneComponent;
}

void ATerrainCluster::MakeMobilityStatic() {
	RootComponent->Mobility = EComponentMobility::Static;
}

void ATerrainCluster::LoadChunk(FInt32Vector2 chunk, uint8 sectionsPerCluster) {
	FInt32Vector2 SectorPosition, ClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	for(int y = 0; y < sectionsPerCluster; y++) {
		for(int x = 0; x < sectionsPerCluster; x++) {
			//UE_LOG(LogTemp, Warning, TEXT("Have to create Section: %dx%d"), SectorPosition.X + x, SectorPosition.Y + y);
			/*FString SectorID = "Sector ";
			SectorID.Append(FString::FromInt(SectorPosition.X + x));
			SectorID.Append("-");
			SectorID.Append(FString::FromInt(SectorPosition.Y + y));*/

			//UTerrainSection* Section = CreateDefaultSubobject<UTerrainSection>(FName(SectorID));
			UTerrainSection* TerrainSection = NewObject<UTerrainSection>(this);
			
			float SectionLocalDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize * FTerrainInfo::SectionsPerCluster;
			FVector SectionLocalPos = FVector(
				x * SectionLocalDimension,
				y * SectionLocalDimension,
				0.0f);
			TerrainSection->AddLocalOffset(SectionLocalPos);
			UE_LOG(LogTemp, Warning, TEXT("Passing Chunk: %dx%d"), chunk.X, chunk.Y);
			TerrainSection->CreateSection(FInt32Vector2(SectorPosition.X + x, SectorPosition.Y + y), FInt32Vector2(chunk.X + x * sectionsPerCluster, chunk.Y + y * sectionsPerCluster),sectionsPerCluster, 64, nullptr);
			TerrainSections.Add(TerrainSection);
		}
	}
}



bool ATerrainCluster::IsChunkLoaded(FInt32Vector2 chunk) {
	FInt32Vector2 SectorPosition, ClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	for(UTerrainSection* Sector : TerrainSections)
		if(Sector->IsSectorLoaded(SectorPosition))
			return true;
	return false;
}

ATerrain* ATerrainCluster::GetTerrain() const {
	return CastChecked<ATerrain>(GetOuter());
}