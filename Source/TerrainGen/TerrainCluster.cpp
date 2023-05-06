// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainCluster.h"
#include "TerrainSection.h"
#include "Terrain.h"

void ATerrainCluster::SafeDestroy() {
	//Release the Components
	//for(TObjectPtr<UTerrainSection> Section : TerrainSections) {
	//	Section->SafeDestroy();
	//	//Section->UnregisterComponent();
	//	//Section->DestroyComponent();
	//	Section->ConditionalBeginDestroy();
	//	Section == nullptr;
	//}

	while(TerrainSections.Num() > 0) {
		TObjectPtr<UTerrainSection> Section = TerrainSections.Pop(false);
		Section->SafeDestroy();
		Section->ConditionalBeginDestroy();
		Section == nullptr;
	}

	//Delete the actor itself
	this->Destroy();
}

ATerrainCluster::ATerrainCluster() {
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent0"));
	RootComponent = SceneComponent;
}

//ATerrainCluster::~ATerrainCluster() {
//	//UE_LOG(LogTemp, Warning, TEXT("Cluster destructor"));
//	Super::~AActor();
//}

void ATerrainCluster::MakeMobilityStatic() {
	RootComponent->Mobility = EComponentMobility::Static;
}

// UNUSED because of redoing of code
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
			//TerrainSection->AddLocalOffset(SectionLocalPos);
			UE_LOG(LogTemp, Warning, TEXT("Passing Chunk: %dx%d"), chunk.X, chunk.Y);
			//TerrainSection->CreateSection(FInt32Vector2(SectorPosition.X + x, SectorPosition.Y + y), FInt32Vector2(chunk.X + x * sectionsPerCluster, chunk.Y + y * sectionsPerCluster), sectionsPerCluster, 64, nullptr);
			TerrainSections.Add(TerrainSection);
		}
	}
}

void ATerrainCluster::LoadAllChunksInCluster() {
	// ClusterPosition;
	//FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	FInt32Vector2 SectorInitialPosition = FInt32Vector2(ClusterBaseX * FTerrainInfo::SectionsPerCluster, ClusterBaseY * FTerrainInfo::SectionsPerCluster);
	int HeightMapSectionSize = (FTerrainInfo::ChunkSize * FTerrainInfo::ChunksPerSection);

	for(int y = 0; y < FTerrainInfo::SectionsPerCluster; y++) {
		for(int x = 0; x < FTerrainInfo::SectionsPerCluster; x++) {
			UTerrainSection* TerrainSection = NewObject<UTerrainSection>(this);

			float SectionLocalDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize * FTerrainInfo::SectionsPerCluster;
			FVector SectionLocalPos = FVector(
				x * SectionLocalDimension,
				y * SectionLocalDimension,
				0.0f);
			//TerrainSection->AddLocalOffset(SectionLocalPos);

			MArray<float> SectionHeightMap = ClusterHeightMap.getArea(x * FTerrainInfo::ChunkSize * FTerrainInfo::ChunksPerSection,
																	  y * FTerrainInfo::ChunkSize * FTerrainInfo::ChunksPerSection,
																	  HeightMapSectionSize,
																	  HeightMapSectionSize);

			//TerrainSection->CreateSection(FInt32Vector2(SectorPosition.X + x, SectorPosition.Y + y), FInt32Vector2(chunk.X + x * sectionsPerCluster, chunk.Y + y * sectionsPerCluster), sectionsPerCluster, 64, nullptr);

			TerrainSection->LoadSection(FInt32Vector2(SectorInitialPosition.X + x, SectorInitialPosition.Y + y), SectionHeightMap);
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

void  ATerrainCluster::HideOutOfRangeChunks(const FVector2D& SphereCenter, float SphereRadius) {
	for(UTerrainSection* Sector : TerrainSections)
		Sector->HideOutOfRangeChunks(SphereCenter, SphereRadius);
}

ATerrain* ATerrainCluster::GetTerrain() const {
	return Owner;
}

void ATerrainCluster::AddChunkToQueue(TUniqueFunction<void()> ChunkLoad) const {
	ATerrain* Terrain = GetTerrain();
	Terrain->EnqueueChunkLoad(MoveTemp(ChunkLoad));
}