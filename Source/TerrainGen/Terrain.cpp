// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "TerrainCluster.h"
#include "TerrainSection.h"
#include <TerrainGen/UtilsArray.h>

#include "Math/NumericLimits.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

// Sets default values
ATerrain::ATerrain() :
	VisionRange(1)
	/*	SectionsPerCluster(2),
		ChunksPerSection(2),
		ChunkSize(64),
		QuadSize(100)*/ {
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Initialize();
}

//// Called when the game starts or when spawned
//void ATerrain::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void ATerrain::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void ATerrain::Initialize() {
	//RootComponent->Mobility = EComponentMobility::Static;
	UE_LOG(LogTemp, Warning, TEXT("Chunk size is: %d"), FTerrainInfo::ChunkSize);
	Reset();
}

void ATerrain::UpdateTerrain(FVector PlayerPosition) {

	Reset();


	FInt32Vector2 sector, cluster;
	FTerrainInfo::ChunkToSectionAndCluster(FInt32Vector2(-3, 1), sector, cluster);
	UE_LOG(LogTemp, Warning, TEXT("Chunk -3 x 1: Sector: %d x %d Cluster: %d x %d"), sector.X, sector.Y, cluster.X, cluster.Y);


	/*UE_LOG(LogTemp, Warning, TEXT("Position: %f     %f     %f"), PlayerPosition.X, PlayerPosition.Y, PlayerPosition.Z);

	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize));
	UE_LOG(LogTemp, Warning, TEXT("Section: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize / ChunksPerSection), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize / ChunksPerSection));
	UE_LOG(LogTemp, Warning, TEXT("Cluster: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster));*/

	FVector startPosition = FVector(1, 1, 0);

	int NumOfChunksRow = FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection * VisionRange;
	int initialChunkX = FMath::FloorToInt(startPosition.X / FTerrainInfo::QuadSize / FTerrainInfo::ChunkSize) - FMath::FloorToInt(VisionRange / 2.0f);
	int initialChunkY = FMath::FloorToInt(startPosition.Y / FTerrainInfo::QuadSize / FTerrainInfo::ChunkSize) - FMath::FloorToInt(VisionRange / 2.0f);
	UE_LOG(LogTemp, Warning, TEXT("initialChunkX %d initialChunkY %d"), initialChunkX, initialChunkY);
	for(int x = initialChunkX; x < initialChunkX + NumOfChunksRow; x++) {
		for(int y = initialChunkY; y < initialChunkY + NumOfChunksRow; y++) {
			if(IsChunkLoaded(FInt32Vector2(x, y))) {
				UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d is loaded."), x, y);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d is not loaded."), x, y);
				LoadChunk(FInt32Vector2(x, y));
				//return;
			}
		}
	}

	//DebugPrintLoadedChunks();

	/*for(ATerrainCluster* Cluster : TerrainClusters) {
		UE::Math::TVector<double> clusterLocation = Cluster->GetTransform().GetLocation();
		UE_LOG(LogTemp, Warning, TEXT("Cluster grid: %d x %d"), Cluster->GetClusterBase().X, Cluster->GetClusterBase().Y);
		UE_LOG(LogTemp, Warning, TEXT("Cluster world: %lf x %lf"), clusterLocation.X, clusterLocation.Y);
	}*/
	for(ATerrainCluster* Cluster : TerrainClusters)
		for(UTerrainSection* Section : Cluster->TerrainSections) {
			FVector sectionLocation = Section->GetComponentTransform().GetLocation();
			UE_LOG(LogTemp, Warning, TEXT("Section grid: %d x %d"), Section->GetSectionBase().X, Section->GetSectionBase().Y);
			UE_LOG(LogTemp, Warning, TEXT("Section world: %lf x %lf"), sectionLocation.X, sectionLocation.Y);
		}
}

void ATerrain::LoadChunk(FInt32Vector2 chunk) {
	// Position of the cluster relative to the center of the world
	//FInt32Vector2 Position = FInt32Vector2(FMath::FloorToInt(chunk.X / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster), FMath::FloorToInt(chunk.Y / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster));
	FInt32Vector2 SectorPosition, ClusterPosition;
	//Check first if it is already loaded
	FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), chunk.X, chunk.Y);
	UE_LOG(LogTemp, Warning, TEXT("Section: %dx%d"), SectorPosition.X, SectorPosition.Y);
	UE_LOG(LogTemp, Warning, TEXT("Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
	UE_LOG(LogTemp, Warning, TEXT(" "));

	if(IsClusterLoaded(ClusterPosition)) {
		//UE_LOG(LogTemp, Warning, TEXT("We already have Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
		return;
	} else {

		FString ClusterID = "TerrainCluster ";
		ClusterID.Append(FString::FromInt(ClusterPosition.X));
		ClusterID.Append(" / ");
		ClusterID.Append(FString::FromInt(ClusterPosition.Y));

		//FGuid name = FGuid::NewGuid();
		//FName(ClusterID)
		//UE_LOG(LogTemp, Warning, TEXT("Have to create Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);

		//ATerrainCluster* NewCluster = NewObject<ATerrainCluster>(this, FName(ClusterID));

		float ClusterWorldDimension =
			FTerrainInfo::ChunkSize *
			FTerrainInfo::QuadSize *
			FTerrainInfo::ChunksPerSection *
			FTerrainInfo::SectionsPerCluster;

		FVector Location(ClusterWorldDimension * ClusterPosition.X, ClusterWorldDimension * ClusterPosition.Y, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;

		ATerrainCluster* NewCluster = GetWorld()->SpawnActor<ATerrainCluster>(Location, Rotation, SpawnInfo);
		//NewCluster->SetRootComponent(this);
		NewCluster->SetActorLabel(ClusterID);

		FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
		//NewCluster->AttachToActor(this, rules);
		NewCluster->MakeMobilityStatic();

		NewCluster->SetClusterBase(ClusterPosition);

		NewCluster->LoadChunk(chunk, FTerrainInfo::SectionsPerCluster);

		TerrainClusters.Add(NewCluster);

	}

}

ATerrainCluster* ATerrain::GetClusterByChunk(FInt32Vector2 chunk) {
	FInt32Vector2 SectorPosition, ClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	for(ATerrainCluster* cluster : TerrainClusters) {
		FInt32Vector2 ClusterBase = cluster->GetClusterBase();
		if(ClusterPosition.X == ClusterBase.X && ClusterPosition.Y == ClusterBase.Y) {
			return cluster;
		}
	}

	return nullptr;
}

UTerrainSection* ATerrain::GetSectionByChunk(FInt32Vector2 chunk) {

	ATerrainCluster* cluster = GetClusterByChunk(chunk);
	FInt32Vector2 SectorPosition, ClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	if(cluster == nullptr)
		return nullptr;

	for(UTerrainSection* section : cluster->TerrainSections) {
		FInt32Vector2 SectionBase = section->GetSectionBase();
		if(SectorPosition.X == SectionBase.X && SectionBase.Y == SectorPosition.Y) {
			return section;
		}
	}

	return nullptr;
}

bool ATerrain::IsClusterLoaded(FInt32Vector2 cluster) {
	//UE_LOG(LogTemp, Warning, TEXT("There are %d clusters active."), TerrainClusters.Num());
	for(ATerrainCluster* TerrainCluster : TerrainClusters) {
		if(TerrainCluster->GetClusterBase() == cluster)
			return true;
	}

	return false;
}

bool ATerrain::IsChunkLoaded(FInt32Vector2 chunk) {
	for(ATerrainCluster* Cluster : TerrainClusters)
		if(Cluster->IsChunkLoaded(FInt32Vector2(chunk.X, chunk.Y)))
			return true;
	return false;
}

void ATerrain::DebugPrintLoadedChunks() {
	int32 minX, minY, maxX, maxY;
	minX = minY = TNumericLimits<int32>::Max();
	maxX = maxY = TNumericLimits<int32>::Min();

	for(ATerrainCluster* Cluster : TerrainClusters)
		for(UTerrainSection* Section : Cluster->TerrainSections) {
			FInt32Vector2 base = Section->GetSectionBase();
			//UE_LOG(LogTemp, Warning, TEXT("Bases: %dx%d"), base.X, base.Y);
			if(base.X < minX) minX = base.X;
			if(base.Y < minY) minY = base.Y;
			if(base.X > maxX) maxX = base.X;
			if(base.Y > maxY) maxY = base.Y;
		}

	int32 sizeX = maxX - minX + 1;
	int32 sizeY = maxY - minY + 1;

	UE_LOG(LogTemp, Warning, TEXT("Max Size: %dx%d"), maxX, maxY);
	UE_LOG(LogTemp, Warning, TEXT("Min Size: %dx%d"), minX, minY);
	UE_LOG(LogTemp, Warning, TEXT("Array Size: %dx%d"), sizeX + 2, sizeY + 2);
	MArray<FString> Table = MArray<FString>("  .  ", sizeX + 2, sizeY + 2);

	for(int x = 1; x < sizeX + 1; x++) {
		FString TableUpperLabel = FString::FromInt(minX + x - 1);
		int32 tailingSpaces = 5 - TableUpperLabel.Len();
		TableUpperLabel = FUtilsArray::AddWhiteSpacesAtStart(tailingSpaces, TableUpperLabel);
		Table.setItem(TableUpperLabel, x, 0);
	}

	for(int y = 1; y < sizeY + 1; y++) {
		FString TableLeftLabel = FString::FromInt(maxY - y + 1);
		int32 tailingSpaces = 5 - TableLeftLabel.Len();
		TableLeftLabel = FUtilsArray::AddWhiteSpacesAtStart(tailingSpaces, TableLeftLabel);
		Table.setItem(TableLeftLabel, 0, y);
	}

	for(int y = 1; y < sizeY + 1; y++) {
		FString TableLeftLabel = FString::FromInt(maxY - y + 1);
		int32 tailingSpaces = 5 - TableLeftLabel.Len();
		TableLeftLabel = FUtilsArray::AddWhiteSpacesAtStart(tailingSpaces, TableLeftLabel);
		Table.setItem(TableLeftLabel, sizeX + 1, y);
	}

	for(int x = 1; x < sizeX + 1; x++) {
		FString TableUpperLabel = FString::FromInt(minX + x - 1);
		int32 tailingSpaces = 5 - TableUpperLabel.Len();
		TableUpperLabel = FUtilsArray::AddWhiteSpacesAtStart(tailingSpaces, TableUpperLabel);
		Table.setItem(TableUpperLabel, x, sizeY + 1);
	}

	for(ATerrainCluster* Cluster : TerrainClusters)
		for(UTerrainSection* Section : Cluster->TerrainSections) {
			FInt32Vector2 base = Section->GetSectionBase();
			Table.setItem(" [x] ", base.X - minX + 1, base.Y - minY + 1); //OFFSET HERE!
		}



	Table.PrintInfo();
	Table.PrintContent();

}

void ATerrain::Reset() {
	TerrainClusters.Empty();
}

void ATerrain::GetHeights(int ChunkPosX, int ChunkPosY, int NumChunksX, int NumChunksY, MArray<float>& HeightMap) {
	HeightMap = MArray<float>(0.0f, ((NumChunksX + 1) * FTerrainInfo::ChunkSize) + 1, ((NumChunksY + 1) * FTerrainInfo::ChunkSize) + 1);
	HeightMap.PrintInfo();

	check(NumChunksX >= 0 || NumChunksY >= 0);
	MArray<float> ChunkHeightMap;
	for(int y = ChunkPosY; y < ChunkPosY + (NumChunksY + 1); y++) {
		for(int x = ChunkPosX; x < ChunkPosX + (NumChunksX + 1); x++) {
			UTerrainSection* section = GetSectionByChunk(FInt32Vector2(x, y));

			if(section == nullptr)
				continue;

			//MArray<float> ChunkHeightMap;
			section->GetChunkHeights(FInt32Vector2(x, y), ChunkHeightMap);
			//ChunkHeightMap.PrintInfo();
			HeightMap.Append(ChunkHeightMap, (x - ChunkPosX) * FTerrainInfo::ChunkSize, (y - ChunkPosY) * FTerrainInfo::ChunkSize);
		}
	}

	HeightMap.PrintInfo();
	ChunkHeightMap.PrintInfo();
	ChunkHeightMap.PrintContent();


}

/*
* This function is for populate whole Chunks, not fine refinement.
*/
void ATerrain::SetHeights(int ChunkPosX, int ChunkPosY, const MArray<float>& HeightMap) {

	//HeightMap.PrintContent();

	FUintVector2 HeightMapSize = HeightMap.GetArraySize();

	//Checks in case there is something wrong with HeightMap
	check(HeightMapSize.X > FTerrainInfo::ChunkSize); // Check if there is a minimum of one chunk in X
	check(HeightMapSize.Y > FTerrainInfo::ChunkSize); // Check if there is a minimum of one chunk in Y
	check(HeightMapSize.X % FTerrainInfo::ChunkSize == 1); // Check if the columns have the correct size to be able to contain a Chunk
	check(HeightMapSize.Y % FTerrainInfo::ChunkSize == 1); // Check if the rows    have the correct size to be able to contain a Chunk

	/*UE_LOG(LogTemp, Warning, TEXT("Columns: %d, ChunkSize: %d Rest: %d"), HeightMapSize.X, FTerrainInfo::ChunkSize, HeightMapSize.X % FTerrainInfo::ChunkSize);
	UE_LOG(LogTemp, Warning, TEXT("Rows: %d, ChunkSize: %d Rest: %d"), HeightMapSize.Y, FTerrainInfo::ChunkSize, HeightMapSize.Y % FTerrainInfo::ChunkSize);*/
	int NumChunksX = HeightMapSize.X / FTerrainInfo::ChunkSize;
	int NumChunksY = HeightMapSize.Y / FTerrainInfo::ChunkSize;
	MArray<float> ChunkHeightMap;
	for(int y = ChunkPosY; y < ChunkPosY + NumChunksY; y++) {
		for(int x = ChunkPosX; x < ChunkPosX + NumChunksX; x++) {
			UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), y, x);
			UE_LOG(LogTemp, Warning, TEXT("Array Position X: %dx%d"), (x - ChunkPosX) * FTerrainInfo::ChunkSize, ((x - ChunkPosX) + 1) * FTerrainInfo::ChunkSize);
			UE_LOG(LogTemp, Warning, TEXT("Array Position Y: %dx%d"), (y - ChunkPosY) * FTerrainInfo::ChunkSize, ((y - ChunkPosY) + 1) * FTerrainInfo::ChunkSize);
			ChunkHeightMap = HeightMap.getArea((x - ChunkPosX) * FTerrainInfo::ChunkSize, 
															 (y - ChunkPosY) * FTerrainInfo::ChunkSize,
															 FTerrainInfo::ChunkSize,
															 FTerrainInfo::ChunkSize);
			//ChunkHeightMap.PrintInfo();

			UTerrainSection* section = GetSectionByChunk(FInt32Vector2(x, y));

			if(section == nullptr)
				continue;

			section->CreateChunkMeshFromHeightMap(FInt32Vector2(x, y), ChunkHeightMap);
		}
	}

	//ChunkHeightMap.PrintContent();

}