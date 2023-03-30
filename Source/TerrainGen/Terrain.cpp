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

	if (cluster == nullptr)
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

	for(int y = ChunkPosY; y < ChunkPosY + (NumChunksY + 1); y++) {
		for(int x = ChunkPosX; x < ChunkPosX + (NumChunksX + 1); x++) {
			UTerrainSection* section = GetSectionByChunk(FInt32Vector2(x, y));

			if(section == nullptr)
				continue;

			MArray<float> ChunkHeightMap;
			section->GetChunkHeights(FInt32Vector2(x, y), ChunkHeightMap);
			ChunkHeightMap.PrintInfo();
			//ChunkHeightMap.PrintContent();
			/*UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 0x0 with value: (%f)"), x, y, ChunkHeightMap.getItem(0,0));
			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 64x0 with value: (%f)"), x, y, ChunkHeightMap.getItem(64, 0));
			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 0x64 with value: (%f)"), x, y, ChunkHeightMap.getItem(0, 64));
			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 64x64 with value: (%f)"), x, y, ChunkHeightMap.getItem(64, 64));*/

			HeightMap.Append(ChunkHeightMap, (x - ChunkPosX) * FTerrainInfo::ChunkSize, (y - ChunkPosY) * FTerrainInfo::ChunkSize);
		}
	}

	HeightMap.PrintInfo();

	/*UE_LOG(LogTemp, Log, TEXT(" --------------------------------- "));

	for(int y = ChunkPosY; y < ChunkPosY + (NumChunksY + 1); y++) {
		for(int x = ChunkPosX; x < ChunkPosX + (NumChunksX + 1); x++) {

			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 0x0 with value: (%f)"), x, y, HeightMap.getItem(0 + ((x - ChunkPosX) * FTerrainInfo::ChunkSize), 0 + (y - ChunkPosY) * FTerrainInfo::ChunkSize));
			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 64x0 with value: (%f)"), x, y, HeightMap.getItem(64 + ((x - ChunkPosX) * FTerrainInfo::ChunkSize), 0 + (y - ChunkPosY) * FTerrainInfo::ChunkSize));
			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 0x64 with value: (%f)"), x, y, HeightMap.getItem(0 + ((x - ChunkPosX) * FTerrainInfo::ChunkSize), 64 + (y - ChunkPosY) * FTerrainInfo::ChunkSize));
			UE_LOG(LogTemp, Log, TEXT("Chunk %dx%d at position 64x64 with value: (%f)"), x, y, HeightMap.getItem(64 + ((x - ChunkPosX) * FTerrainInfo::ChunkSize), 64 + (y - ChunkPosY) * FTerrainInfo::ChunkSize));

		}
	}*/



	//FInt32Vector2 SectorInitialPosition, ClusterInitialPosition;
	//FInt32Vector2 SectorFinalPosition, ClusterFinalPosition;
	//FInt32Vector2 ChunkVector(ChunkPosX + NumChunksX, ChunkPosY + NumChunksY);


	//FTerrainInfo::ChunkToSectionAndCluster(FInt32Vector2(ChunkPosX, ChunkPosY), SectorInitialPosition, ClusterInitialPosition);
	//FTerrainInfo::ChunkToSectionAndCluster(ChunkVector, SectorFinalPosition, ClusterFinalPosition);

	//UE_LOG(LogTemp, Warning, TEXT("Initial: %dx%d Final: %dx%d"),
	//	   SectorInitialPosition.X, SectorInitialPosition.Y,
	//	   SectorFinalPosition.X, SectorFinalPosition.Y);


	//// Iterate over all TerrainSections that intersect with the requested area
	//for(ATerrainCluster* cluster : TerrainClusters) {
	//	for(UTerrainSection* section : cluster->TerrainSections) {
	//		FInt32Vector2 SectorBase = section->GetSectionBase();
	//		UE_LOG(LogTemp, Warning, TEXT("Base: %dx%d"), SectorBase.X, SectorBase.Y);

	//		if(SectorBase.X < SectorInitialPosition.X || SectorBase.X > SectorFinalPosition.X &&
	//		   SectorBase.Y < SectorInitialPosition.Y || SectorBase.Y > SectorFinalPosition.Y) {
	//			UE_LOG(LogTemp, Warning, TEXT("Got in"));
	//			// This section does not intersect with the requested area
	//			continue;
	//		}



	//		/*FInt32Vector2 ChunkBase;
	//		FTerrainInfo::SectionToChunkAndCluster(SectorBase, ChunkBase, ClusterInitialPosition);
	//		for(int y = ChunkBase.Y; y < ChunkBase.Y + 1; y++) {
	//			for(int x = ChunkBase.X; x < ChunkBase.X + 1; x++) {
	//				if(ChunkBase.X >= ChunkPosX && ChunkBase.X <= ChunkVector.X &&
	//				   ChunkBase.Y >= ChunkPosY && ChunkBase.Y <= ChunkVector.Y) {

	//					MArray<float> ChunkHeightMap;
	//					section->GetChunkHeights(FInt32Vector2(x, y), ChunkHeightMap);

	//				}
	//			}
	//		}*/
	//		// Hacer que por cada Chunk busque el Cluster, y después el Sector, así está más robusto, mas lento, pero robusto.

	//		//for(int y = ChunkPosY; y < ChunkPosY + (NumChunksY + 1); y++) {
	//			//for(int x = ChunkPosX; x < ChunkPosX + (NumChunksX + 1); x++) {
	//		MArray<float> ChunkHeightMap;
	//		//FInt32Vector2 ChunkLocalPos = FInt32Vector2(x, y);

	//		section->GetChunkHeights(FInt32Vector2(ChunkPosX, ChunkPosY), ChunkHeightMap);

	//		//int OffsetX = ChunkLocalPos.X * FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize;
	//		//int OffsetY = ChunkLocalPos.Y * FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize;
	//		UE_LOG(LogTemp, Warning, TEXT("We are at %dx%d"), ChunkPosY, ChunkPosY);
	//		HeightMap.Append(ChunkHeightMap, ChunkPosX * FTerrainInfo::ChunkSize, ChunkPosY * FTerrainInfo::ChunkSize);
	//		//}
	//	//}


	//	}
	//}

	// Iterate over all TerrainSections that intersect with the requested area
	//for(ATerrainCluster* cluster : TerrainClusters) {
	//	for(UTerrainSection* section : cluster->TerrainSections) {
	//		int sectionMinX = section->GetActorLocation().X - chunkSize / 2;
	//		int sectionMinY = section->GetActorLocation().Y - chunkSize / 2;
	//		int sectionMaxX = sectionMinX + chunkSize;
	//		int sectionMaxY = sectionMinY + chunkSize;

	//		if(posX + sizeX <= sectionMinX || posY + sizeY <= sectionMinY ||
	//		   posX >= sectionMaxX || posY >= sectionMaxY) {
	//			// This section does not intersect with the requested area
	//			continue;
	//		}

	//		// Iterate over all chunks in this section that intersect with the requested area
	//		for(UStaticMeshComponent* chunk : section->ChunkStaticMesh) {
	//			int chunkMinX = chunk->GetComponentLocation().X - chunkSize / 2;
	//			int chunkMinY = chunk->GetComponentLocation().Y - chunkSize / 2;
	//			int chunkMaxX = chunkMinX + chunkSize;
	//			int chunkMaxY = chunkMinY + chunkSize;

	//			if(posX + sizeX <= chunkMinX || posY + sizeY <= chunkMinY ||
	//			   posX >= chunkMaxX || posY >= chunkMaxY) {
	//				// This chunk does not intersect with the requested area
	//				continue;
	//			}

	//			// Get the heights from this chunk
	//			UStaticMesh* mesh = chunk->GetStaticMesh();
	//			TArray<FVector> vertices;
	//			//mesh->GetSection(0)->GetVertices(vertices);

	//			int startX = FMath::Max(0, posX - chunkMinX);
	//			int startY = FMath::Max(0, posY - chunkMinY);
	//			int endX = FMath::Min(chunkSize, posX + sizeX - chunkMinX);
	//			int endY = FMath::Min(chunkSize, posY + sizeY - chunkMinY);

	//			for(int x = startX; x < endX; x++) {
	//				for(int y = startY; y < endY; y++) {
	//					int index = y * chunkSize + x;
	//					float height = vertices[index].Z;
	//					int arrayX = posX + x - chunkMinX;
	//					int arrayY = posY + y - chunkMinY;
	//					heights.setItem(height, arrayX, arrayY);
	//				}
	//			}
	//		}
	//	}
	//}
}