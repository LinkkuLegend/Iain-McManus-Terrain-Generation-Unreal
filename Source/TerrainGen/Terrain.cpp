// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "TerrainCluster.h"
#include "TerrainSection.h"
#include <TerrainGen/UtilsArray.h>
#include "WorldTerrainGen.h"

#include "Async/Async.h"

#include "Math/NumericLimits.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ATerrain::ATerrain() :
	VisionRange(16)
	/*	SectionsPerCluster(2),
		ChunksPerSection(2),
		ChunkSize(64),
		QuadSize(100)*/ {
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Initialize();
}

// Called when the game starts or when spawned
void ATerrain::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Reached Begin play in ATerrain"));
	Super::BeginPlay();
	//LoadNextChunkFromQueue();
	UE_LOG(LogTemp, Warning, TEXT("Reached Begin play in ATerrain"));
}
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
	UE_LOG(LogTemp, Warning, TEXT("So nice!5"));
	//Reset();
}

void ATerrain::UpdateTerrain(const FVector PlayerPosition) {

	FDateTime StartTime = FDateTime::Now();

	UE_LOG(LogTemp, Warning, TEXT("Player location: %fx%fx%f"), PlayerPosition.X, PlayerPosition.Y, PlayerPosition.Z);

	int initialChunkX = FMath::FloorToInt(PlayerPosition.X / FTerrainInfo::QuadSize / FTerrainInfo::ChunkSize);
	int initialChunkY = FMath::FloorToInt(PlayerPosition.Y / FTerrainInfo::QuadSize / FTerrainInfo::ChunkSize);


	/*FInt32Vector2 SectorPosition, ClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(FInt32Vector2(initialChunkX, initialChunkX), SectorPosition, ClusterPosition);*/

	// The idea is to use the VisionRange to calcule the area of chunks we need, so that way we can calculate which Clusters are involved
	//				  ----* <- FinalChunk	
	//				  |   |
	// InitialChunk-> *----	
	FInt32Vector2 SWChunk = FInt32Vector2(initialChunkX - VisionRange + 1, initialChunkY - VisionRange + 1);
	FInt32Vector2 NEChunk = FInt32Vector2(initialChunkX + VisionRange - 1, initialChunkY + VisionRange - 1);

	UE_LOG(LogTemp, Warning, TEXT("InitialChunkX: %d, InitialChunkY: %d"), initialChunkX, initialChunkY);
	UE_LOG(LogTemp, Warning, TEXT("SWChunk: %d, SWChunk: %d"), SWChunk.X, SWChunk.Y);
	UE_LOG(LogTemp, Warning, TEXT("NEChunk: %d, NEChunk: %d"), NEChunk.X, NEChunk.Y);

	FInt32Vector2 SWSectorPosition, SWClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(SWChunk, SWSectorPosition, SWClusterPosition);

	FInt32Vector2 NESectorPosition, NEClusterPosition;
	FTerrainInfo::ChunkToSectionAndCluster(NEChunk, NESectorPosition, NEClusterPosition);

	UE_LOG(LogTemp, Warning, TEXT("SWCluster: %d, SWCluster: %d"), SWClusterPosition.X, SWClusterPosition.Y);
	UE_LOG(LogTemp, Warning, TEXT("NECluster: %d, NECluster: %d"), NEClusterPosition.X, NEClusterPosition.Y);

	TArray< FInt32Vector2> ClustersNeededToLoad;

	/*for(int i = SWClusterPosition.X; i < NEClusterPosition.X + 1; i++) {
		for(int j = SWClusterPosition.Y; j < NEClusterPosition.Y + 1; j++) {
			UE_LOG(LogTemp, Warning, TEXT("Need to load Cluster %dx%d"), i, j);
			ClustersNeededToLoad.Add(FInt32Vector2(i, j));
			LoadClusterAsync(FInt32Vector2(i, j));
		}
	}*/

	TArray<FInt32Vector2> TestSpiral;
	PopulateArrayInSpiralOrder(SWClusterPosition, NEClusterPosition, TestSpiral);

	while(!TestSpiral.IsEmpty()) {
		FInt32Vector2 ClusterSpiral;
		ClusterSpiral = TestSpiral.Pop(false);

		UE_LOG(LogTemp, Warning, TEXT("Need to load Cluster %dx%d"), ClusterSpiral.X, ClusterSpiral.Y);
		ClustersNeededToLoad.Add(ClusterSpiral);
		LoadClusterAsync(ClusterSpiral);
	}


	//Now we are gonna save and unload out-of-range Clusters
	TArray< ATerrainCluster*> ClusterToSaveAndDestroy;
	for(ATerrainCluster* Cluster : TerrainClusters) {
		bool ContainsCluster = false;
		for(FInt32Vector2 ClusterToLoad : ClustersNeededToLoad) {
			if(Cluster->GetClusterBase().X == ClusterToLoad.X &&
			   Cluster->GetClusterBase().Y == ClusterToLoad.Y)
				ContainsCluster = true;
		}
		if(!ContainsCluster) {
			UE_LOG(LogTemp, Warning, TEXT("We are gonna destroy Cluster: %dx%d"), Cluster->GetClusterBase().X, Cluster->GetClusterBase().Y);
			Cluster->SafeDestroy();
			ClusterToSaveAndDestroy.Add(Cluster);
		}
	}

	// And remove the pointer from the array
	for(ATerrainCluster* ClusterToRemove : ClusterToSaveAndDestroy) {
		TerrainClusters.Remove(ClusterToRemove);
	}

	//HideOutOfRangeChunks(FVector2D(PlayerPosition.X, PlayerPosition.Y), GetVisionRadius());

	//UE_LOG(LogTemp, Warning, TEXT("Cluster in array: %d"), TerrainClusters.Num());

	FDateTime EndTime = FDateTime::Now();
	const FTimespan Duration2 = EndTime - StartTime;
	UE_LOG(LogTemp, Warning, TEXT("Update took: %f seconds"), Duration2.GetTotalSeconds());

}

//void ATerrain::UpdateTerrain(FVector PlayerPosition) {
//
//	//Reset();
//
//	/*FInt32Vector2 sector, cluster;
//	FTerrainInfo::ChunkToSectionAndCluster(FInt32Vector2(-3, 1), sector, cluster);
//	UE_LOG(LogTemp, Warning, TEXT("Chunk -3 x 1: Sector: %d x %d Cluster: %d x %d"), sector.X, sector.Y, cluster.X, cluster.Y);
//*/
//
//	/*UE_LOG(LogTemp, Warning, TEXT("Position: %f     %f     %f"), PlayerPosition.X, PlayerPosition.Y, PlayerPosition.Z);
//
//	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize));
//	UE_LOG(LogTemp, Warning, TEXT("Section: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize / ChunksPerSection), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize / ChunksPerSection));
//	UE_LOG(LogTemp, Warning, TEXT("Cluster: %dx%d"), FMath::FloorToInt(PlayerPosition.X / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster), FMath::FloorToInt(PlayerPosition.Y / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster));*/
//
//	//FVector startPosition = FVector(1, 1, 0);
//	FVector startPosition = PlayerPosition;
//
//	int NumOfChunksRow = FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection * VisionRange;
//	int initialChunkX = FMath::FloorToInt(startPosition.X / FTerrainInfo::QuadSize / FTerrainInfo::ChunkSize) - FMath::FloorToInt(VisionRange / 2.0f);
//	int initialChunkY = FMath::FloorToInt(startPosition.Y / FTerrainInfo::QuadSize / FTerrainInfo::ChunkSize) - FMath::FloorToInt(VisionRange / 2.0f);
//	UE_LOG(LogTemp, Warning, TEXT("InitialChunkX: %d, InitialChunkY: %d, NumOfChunksRow: %d"), initialChunkX, initialChunkY, NumOfChunksRow);
//	for(int x = initialChunkX - NumOfChunksRow; x < initialChunkX + NumOfChunksRow; x++) {
//		for(int y = initialChunkY - NumOfChunksRow; y < initialChunkY + NumOfChunksRow; y++) {
//			if(IsChunkLoaded(FInt32Vector2(x, y))) {
//				UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d is loaded."), x, y);
//			} else {
//				UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d is not loaded."), x, y);
//					LoadChunk(FInt32Vector2(x, y));
//				
//				//return;
//			}
//		}
//	}
//
//	//DebugPrintLoadedChunks();
//
//	/*for(ATerrainCluster* Cluster : TerrainClusters) {
//		UE::Math::TVector<double> clusterLocation = Cluster->GetTransform().GetLocation();
//		UE_LOG(LogTemp, Warning, TEXT("Cluster grid: %d x %d"), Cluster->GetClusterBase().X, Cluster->GetClusterBase().Y);
//		UE_LOG(LogTemp, Warning, TEXT("Cluster world: %lf x %lf"), clusterLocation.X, clusterLocation.Y);
//	}*/
//
//	/*for(ATerrainCluster* Cluster : TerrainClusters)
//		for(UTerrainSection* Section : Cluster->TerrainSections) {
//			FVector sectionLocation = Section->GetComponentTransform().GetLocation();
//			UE_LOG(LogTemp, Warning, TEXT("Section grid: %d x %d"), Section->GetSectionBase().X, Section->GetSectionBase().Y);
//			UE_LOG(LogTemp, Warning, TEXT("Section world: %lf x %lf"), sectionLocation.X, sectionLocation.Y);
//		}*/
//}

void ATerrain::LoadClusterAsync(FInt32Vector2 cluster) {

	// First we are gonna check it doesn't already exists
	if(IsClusterLoaded(cluster))
		return;

	// Name of the cluster for the editor
	FString ClusterID = "TerrainCluster ";
	ClusterID.Append(FString::FromInt(cluster.X));
	ClusterID.Append(" / ");
	ClusterID.Append(FString::FromInt(cluster.Y));

	// Total dimension of the cluster in one of the axis (it is mean to be square)
	float ClusterWorldDimension =
		FTerrainInfo::ChunkSize *
		FTerrainInfo::QuadSize *
		FTerrainInfo::ChunksPerSection *
		FTerrainInfo::SectionsPerCluster;

	// Final location of the cluster in the world
	FVector Location(ClusterWorldDimension * cluster.X, ClusterWorldDimension * cluster.Y, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);

	// For the object construction, we need a owner, so lets say it is this object
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;



	ATerrainCluster* NewCluster = GetWorld()->SpawnActor<ATerrainCluster>(Location, Rotation, SpawnInfo);

	NewCluster->Owner = this;

	//NewCluster->SetRootComponent(this);
	NewCluster->SetActorLabel(ClusterID);

	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
	//NewCluster->AttachToActor(this, rules);
	NewCluster->MakeMobilityStatic();

	NewCluster->SetClusterBase(cluster);

	NewCluster->SetClusterHeightMap(WorldTerrainGen::GetClusterHeights(FIntPoint(cluster.X, cluster.Y)));

	NewCluster->LoadAllChunksInCluster();

	TerrainClusters.Add(NewCluster);


	UE_LOG(LogTemp, Warning, TEXT("Cluster data generated."));

}

void ATerrain::LoadNextChunkFromQueue() {
	if(!ChunkQueueLoad.IsEmpty()) {
		// Get the next chunk function from the queue
		TUniqueFunction<void()> ResolutionAsyncFunction;
		ChunkQueueLoad.Dequeue(ResolutionAsyncFunction);

		// Load the chunk at the given position asynchronously
		FGraphEventRef MyTask = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(ResolutionAsyncFunction), TStatId(), nullptr, ENamedThreads::GameThread);

		
	}

	// Set a delay before loading the next chunk
	const float ChunkLoadDelay = 0.000001f; // Adjust this value as needed
	GetWorld()->GetTimerManager().SetTimer(ChunkLoadTimerHandle, this, &ATerrain::LoadNextChunkFromQueue, ChunkLoadDelay, false);

	//const float ChunkLoadDelay = 0.1f; // Adjust this value as needed
	//GetWorld()->GetTimerManager().SetTimer(ChunkLoadTimerHandle, this, &ATerrain::LoadNextChunkFromQueue, ChunkLoadDelay, false);

	//UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
}


//void ATerrain::LoadChunk(FInt32Vector2 chunk) {
//	// Position of the cluster relative to the center of the world
//	//FInt32Vector2 Position = FInt32Vector2(FMath::FloorToInt(chunk.X / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster), FMath::FloorToInt(chunk.Y / QuadSize / ChunkSize / ChunksPerSection / SectionsPerCluster));
//	FInt32Vector2 SectorPosition, ClusterPosition;
//	//Check first if it is already loaded
//	FTerrainInfo::ChunkToSectionAndCluster(chunk, SectorPosition, ClusterPosition);
//
//	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), chunk.X, chunk.Y);
//	UE_LOG(LogTemp, Warning, TEXT("Section: %dx%d"), SectorPosition.X, SectorPosition.Y);
//	UE_LOG(LogTemp, Warning, TEXT("Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
//	UE_LOG(LogTemp, Warning, TEXT(" "));
//
//	if(IsClusterLoaded(ClusterPosition)) {
//		//UE_LOG(LogTemp, Warning, TEXT("We already have Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
//		return;
//	} else {
//
//		FString ClusterID = "TerrainCluster ";
//		ClusterID.Append(FString::FromInt(ClusterPosition.X));
//		ClusterID.Append(" / ");
//		ClusterID.Append(FString::FromInt(ClusterPosition.Y));
//
//		//FGuid name = FGuid::NewGuid();
//		//FName(ClusterID)
//		//UE_LOG(LogTemp, Warning, TEXT("Have to create Cluster: %dx%d"), ClusterPosition.X, ClusterPosition.Y);
//
//		//ATerrainCluster* NewCluster = NewObject<ATerrainCluster>(this, FName(ClusterID));
//
//		float ClusterWorldDimension =
//			FTerrainInfo::ChunkSize *
//			FTerrainInfo::QuadSize *
//			FTerrainInfo::ChunksPerSection *
//			FTerrainInfo::SectionsPerCluster;
//
//		FVector Location(ClusterWorldDimension * ClusterPosition.X, ClusterWorldDimension * ClusterPosition.Y, 0.0f);
//		FRotator Rotation(0.0f, 0.0f, 0.0f);
//		FActorSpawnParameters SpawnInfo;
//		SpawnInfo.Owner = this;
//
//		ATerrainCluster* NewCluster = GetWorld()->SpawnActor<ATerrainCluster>(Location, Rotation, SpawnInfo);
//		//NewCluster->SetRootComponent(this);
//		NewCluster->SetActorLabel(ClusterID);
//
//		FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
//		//NewCluster->AttachToActor(this, rules);
//		NewCluster->MakeMobilityStatic();
//
//		NewCluster->SetClusterBase(ClusterPosition);
//
//		NewCluster->LoadChunk(chunk, FTerrainInfo::SectionsPerCluster);
//
//		TerrainClusters.Add(NewCluster);
//
//	}
//
//}

void ATerrain::HideOutOfRangeChunks(const FVector2D& PlayerLocation, float VisionRangeRadius) {
	for(ATerrainCluster* Cluster : TerrainClusters)
		Cluster->HideOutOfRangeChunks(PlayerLocation, VisionRangeRadius);
}

void ATerrain::PopulateArrayInSpiralOrder(FInt32Vector2 PointSW, FInt32Vector2 PointNE, TArray<FInt32Vector2>& SpiralGrid) {
	//TQueue<FInt32Vector2> SpiralGrid;


	int32 Rows = PointNE.Y - PointSW.Y;
	int32 Cols = PointNE.X - PointSW.X;

	int32 Top = Rows;
	int32 Bottom = 0;
	int32 Left = 0;
	int32 Right = Cols;

	while(Bottom <= Top && Left <= Right) {
		// Traverse right
		for(int32 i = Left; i <= Right; ++i) {
			SpiralGrid.Push(FInt32Vector2(PointSW.X + i, PointSW.Y + Bottom));
		}
		Bottom++;

		// Traverse up
		for(int32 i = Bottom; i <= Top; ++i) {
			SpiralGrid.Push(FInt32Vector2(PointSW.X + Right, PointSW.Y + i));
		}
		Right--;

		// Traverse left
		if(Bottom <= Top) {
			for(int32 i = Right; i >= Left; --i) {
				SpiralGrid.Push(FInt32Vector2(PointSW.X + i, PointSW.Y + Top));
			}
			Top--;
		}

		// Traverse down
		if(Left <= Right) {
			for(int32 i = Top; i >= Bottom; --i) {
				SpiralGrid.Push(FInt32Vector2(PointSW.X + Left, PointSW.Y + i));
			}
			Left++;
		}
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
	for(ATerrainCluster* TerrainCluster : TerrainClusters)
		if(TerrainCluster->GetClusterBase() == cluster)
			return true;
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
	UE_LOG(LogTemp, Warning, TEXT("Resetting..."));
	// Get all Actors of the specified class
	//TArray<AActor*> ActorsToDelete;
	//TSubclassOf<ATerrainCluster> ClusterClass = ATerrainCluster::StaticClass();
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClusterClass, ActorsToDelete);

	//// Delete each Actor
	//for(AActor* Actor : ActorsToDelete) {
	//	TArray<UStaticMeshComponent*> Components;
	//	UE_LOG(LogTemp, Warning, TEXT("Found it..."));
	//	Actor->GetComponents<UStaticMeshComponent>(Components);

	//	for(UStaticMeshComponent* Component : Components) {
	//		Component->UnregisterComponent();
	//		UE_LOG(LogTemp, Warning, TEXT("Unregistering..."));
	//	}

	//}
	// 
	// Get all actors of a specific class
	TArray<AActor*> Actors;
	TSubclassOf<ATerrainCluster> ClusterClass = ATerrainCluster::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GWorld, ClusterClass, Actors);

	// Delete all actors of that class
	for(AActor* Actor : Actors) {
		Actor->Destroy();
	}



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
			//section->GetChunkHeights(FInt32Vector2(x, y), ChunkHeightMap);
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
* ChunkPosX and ChunkPosY are the starting Chunks, HeightMap are meant to contain more than one chunk
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

			//section->CreateChunkMeshFromHeightMap(FInt32Vector2(x, y), ChunkHeightMap);
		}
	}

	//ChunkHeightMap.PrintContent();

}

/*
* This function is for populate whole Chunks, not fine refinement.
* ChunkPosX and ChunkPosY are the starting Chunks, HeightMap are meant to contain more than one chunk
*/
void ATerrain::SetTextures(int ChunkPosX, int ChunkPosY, const MArray<uint8>& BiomeMap) {

	FUintVector2 HeightMapSize = BiomeMap.GetArraySize();

	//Checks in case there is something wrong with HeightMap
	check(HeightMapSize.X > FTerrainInfo::ChunkSize); // Check if there is a minimum of one chunk in X
	check(HeightMapSize.Y > FTerrainInfo::ChunkSize); // Check if there is a minimum of one chunk in Y
	check(HeightMapSize.X % FTerrainInfo::ChunkSize == 1); // Check if the columns have the correct size to be able to contain a Chunk
	check(HeightMapSize.Y % FTerrainInfo::ChunkSize == 1); // Check if the rows    have the correct size to be able to contain a Chunk

	/*UE_LOG(LogTemp, Warning, TEXT("Columns: %d, ChunkSize: %d Rest: %d"), HeightMapSize.X, FTerrainInfo::ChunkSize, HeightMapSize.X % FTerrainInfo::ChunkSize);
	UE_LOG(LogTemp, Warning, TEXT("Rows: %d, ChunkSize: %d Rest: %d"), HeightMapSize.Y, FTerrainInfo::ChunkSize, HeightMapSize.Y % FTerrainInfo::ChunkSize);*/
	int NumChunksX = HeightMapSize.X / FTerrainInfo::ChunkSize;
	int NumChunksY = HeightMapSize.Y / FTerrainInfo::ChunkSize;
	MArray<uint8> ChunkTexturesMap;
	for(int y = ChunkPosY; y < ChunkPosY + NumChunksY; y++) {
		for(int x = ChunkPosX; x < ChunkPosX + NumChunksX; x++) {
			UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), y, x);
			UE_LOG(LogTemp, Warning, TEXT("Array Position X: %dx%d"), (x - ChunkPosX) * FTerrainInfo::ChunkSize, ((x - ChunkPosX) + 1) * FTerrainInfo::ChunkSize);
			UE_LOG(LogTemp, Warning, TEXT("Array Position Y: %dx%d"), (y - ChunkPosY) * FTerrainInfo::ChunkSize, ((y - ChunkPosY) + 1) * FTerrainInfo::ChunkSize);
			ChunkTexturesMap = BiomeMap.getArea((x - ChunkPosX) * FTerrainInfo::ChunkSize,
												(y - ChunkPosY) * FTerrainInfo::ChunkSize,
												FTerrainInfo::ChunkSize,
												FTerrainInfo::ChunkSize);
			//ChunkHeightMap.PrintInfo();

			UTerrainSection* section = GetSectionByChunk(FInt32Vector2(x, y));

			if(section == nullptr)
				continue;

			section->CreateMaterialsFromBiomeMap(FInt32Vector2(x, y), ChunkTexturesMap);
		}
	}

	//ChunkHeightMap.PrintContent();

}


int ATerrain::GetVisionRadius() {
	return VisionRange * FTerrainInfo::QuadSize * FTerrainInfo::ChunkSize / 2;
}