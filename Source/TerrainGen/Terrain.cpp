// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "TerrainCluster.h"
#include "TerrainSection.h"
#include <TerrainGen/UtilsDataStructs.h>
#include <TerrainGen/UtilsArray.h>

#include "Math/NumericLimits.h"


// Sets default values
ATerrain::ATerrain() :
	VisionRange(4)
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
	UE_LOG(LogTemp, Warning, TEXT("Chunk size is: %d"), ChunkSize);
	Reset();
}

void ATerrain::UpdateTerrain(FVector PlayerPosition) {

	Reset();

	/*UE_LOG(LogTemp, Warning, TEXT("Position: %f     %f     %f"), PlayerPosition.X, PlayerPosition.Y, PlayerPosition.Z);

	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize));
	UE_LOG(LogTemp, Warning, TEXT("Section: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize / ChunksPerSection), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize / ChunksPerSection));
	UE_LOG(LogTemp, Warning, TEXT("Cluster: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster));*/

	FVector startPosition = FVector(1, 1, 0);

	int NumOfChunks = SectionsPerCluster * ChunksPerSection * VisionRange;
	int NumOfSections = ChunksPerSection * VisionRange;
	int initialX = FMath::FloorToInt(startPosition.X / QuadSize / ChunkSize) - NumOfChunks / 2;
	int initialY = FMath::FloorToInt(startPosition.Y / QuadSize / ChunkSize) - NumOfChunks / 2;
	UE_LOG(LogTemp, Warning, TEXT("initialX %d initialY %d"), initialX, initialY);
	for(int x = initialX; x < initialX + NumOfChunks; x++) {
		for(int y = initialY; y < initialY + NumOfChunks; y++) {
			if(IsChunkLoaded(FInt32Vector2(x, y))) {
				UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d is loaded."), x, y);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d is not loaded."), x, y);
				LoadChunk(FInt32Vector2(x, y));
			}
		}
	}

	DebugPrintLoadedChunks();
}

void ATerrain::LoadChunk(FInt32Vector2 chunk) {
	// Position of the cluster relative to the center of the world
	//FInt32Vector2 Position = FInt32Vector2(FMath::FloorToInt(chunk.X / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster), FMath::FloorToInt(chunk.Y / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster));
	FInt32Vector2 SectorPosition, ClusterPosition;
	//Check first if it is already loaded
	ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);

	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), chunk.X, chunk.Y);
	UE_LOG(LogTemp, Warning, TEXT("Section: %dx%d"), SectorPosition.X, SectorPosition.Y);
	UE_LOG(LogTemp, Warning, TEXT("Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
	UE_LOG(LogTemp, Warning, TEXT(" "));

	if(IsClusterLoaded(ClusterPosition)) {
		//UE_LOG(LogTemp, Warning, TEXT("We already have Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
		return;
	} else {

		FString ClusterID = "Cluster ";
		ClusterID.Append(FString::FromInt(ClusterPosition.X));
		ClusterID.Append("-");
		ClusterID.Append(FString::FromInt(ClusterPosition.Y));

		//FGuid name = FGuid::NewGuid();
		//FName(ClusterID)
		//UE_LOG(LogTemp, Warning, TEXT("Have to create Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);

		ATerrainCluster* NewCluster = NewObject<ATerrainCluster>(this, FName(ClusterID));
		NewCluster->SetClusterBase(ClusterPosition);
		NewCluster->LoadChunk(chunk, SectionsPerCluster);

		TerrainClusters.Add(NewCluster);

	}

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

void ATerrain::ChunkToSectionAndCluster(FInt32Vector2 chunk, FInt32Vector2& sector, FInt32Vector2& cluster) {
	int sectionX, sectionY;
	int clusterX, clusterY;

	float sectionFactor = (ChunksPerSection * 1.0f);
	float clusterFactor = (ChunksPerSection * SectionsPerCluster * 1.0f);


	sectionX = FMath::FloorToInt(chunk.X / sectionFactor);
	clusterX = FMath::FloorToInt(chunk.X / clusterFactor);

	sectionY = FMath::FloorToInt(chunk.Y / sectionFactor);
	clusterY = FMath::FloorToInt(chunk.Y / clusterFactor);

	sector = FInt32Vector2(sectionX, sectionY);
	cluster = FInt32Vector2(clusterX, clusterY);
}