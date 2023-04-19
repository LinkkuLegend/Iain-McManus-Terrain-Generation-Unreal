// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainSection.h"
#include "TerrainCluster.h"
#include "Terrain.h"

#include "Misc/DateTime.h"
#include "Math/UnrealMathUtility.h"

#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshOperations.h"
#include "StaticMeshAttributes.h"
#include <Runtime/MeshConversion/Public/MeshDescriptionBuilder.h>



void UTerrainSection::LoadSection(FInt32Vector2 SectionPos, UTexture2D* heightMap) {

	SectionBaseX = SectionPos.X;
	SectionBaseY = SectionPos.Y;

	NumChunks = FTerrainInfo::SectionsPerCluster;
	ChunkNumQuads = FTerrainInfo::ChunkSize - 1;
	TotalNumQuads = ChunkNumQuads * (NumChunks ^ 2);

	//FGuid name = FGuid::NewGuid();

	int SectionLocalDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize * FTerrainInfo::SectionsPerCluster;
	int ChunkDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize;

	FInt32Vector2 FirstChunkInSection = FInt32Vector2(SectionPos.X * FTerrainInfo::ChunksPerSection, SectionPos.Y * FTerrainInfo::ChunksPerSection);

	//Procedural Mesh version

	//for(int ChunkSubsectionY = 0; ChunkSubsectionY < NumChunks; ChunkSubsectionY++) {
	//	for(int ChunkSubsectionX = 0; ChunkSubsectionX < NumChunks; ChunkSubsectionX++) {
	//		UE_LOG(LogTemp, Log, TEXT("Start loop: %f seconds"), (FDateTime::UtcNow() - StartTime).GetTotalSeconds());
	//		int ChunkSubsection = ChunkSubsectionX + ChunkSubsectionY * NumChunks;

	//		FVector SectionLocalPos = FVector(
	//			SectionPos.X * SectionLocalDimension + ChunkSubsectionX * ChunkDimension,
	//			SectionPos.Y * SectionLocalDimension + ChunkSubsectionY * ChunkDimension,
	//			0.0f);

	//		FString ChunkID = "Chunk ";
	//		ChunkID.Append(FString::FromInt(FirstChunkInSection.X + ChunkSubsectionX));
	//		ChunkID.Append("-");
	//		ChunkID.Append(FString::FromInt(FirstChunkInSection.Y + ChunkSubsectionY));

	//		ChunkMesh.Add(NewObject<UProceduralMeshComponent>(this, FName(ChunkID)));
	//		ChunkMesh[ChunkSubsection]->SetWorldLocation(SectionLocalPos);
	//		ChunkMesh[ChunkSubsection]->Mobility = EComponentMobility::Static;

	//		TArray<FVector> Vertices, Normals;
	//		TArray<int> Triangles;
	//		TArray<FVector2D> UV0;
	//		for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
	//			for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {
	//				Vertices.Add(FVector(x * FTerrainInfo::QuadSize, y * FTerrainInfo::QuadSize, 0));
	//				Normals.Add(FVector(0, 0, 1));
	//				UV0.Add(FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)));
	//			}
	//		}

	//		//UE_LOG(LogTemp, Warning, TEXT("Num: %d"), Vertices.Num());
	//		//UE_LOG(LogTemp, Warning, TEXT("Ours: %d"), FTerrainInfo::ChunkSize * FTerrainInfo::ChunkSize);
	//		int RowCurrent, RowNext;
	//		for(int y = 0; y < FTerrainInfo::ChunkSize; y++) {
	//			for(int x = 0; x < FTerrainInfo::ChunkSize; x++) {
	//				RowCurrent = x + (FTerrainInfo::ChunkSize + 1) * y;
	//				RowNext = RowCurrent + FTerrainInfo::ChunkSize + 1;
	//				Triangles.Add(RowCurrent);
	//				Triangles.Add(RowNext);
	//				Triangles.Add(RowCurrent + 1);
	//				Triangles.Add(RowCurrent + 1);
	//				Triangles.Add(RowNext);
	//				Triangles.Add(RowNext + 1);
	//			}
	//		}
	//		ChunkMesh[ChunkSubsection]->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	//		ChunkMesh[ChunkSubsection]->RegisterComponent();
	//		GetCluster()->AddInstanceComponent(ChunkMesh[ChunkSubsection]);

	//		UE_LOG(LogTemp, Log, TEXT("End loop: %f seconds"), (FDateTime::UtcNow() - StartTime).GetTotalSeconds());
	//	}
	//}

	FInt32Vector2 Chunk, Cluster;
	FTerrainInfo::SectionToChunkAndCluster(SectionPos, Chunk, Cluster);

	//Static mesh version

	for(int ChunkSubsectionY = 0; ChunkSubsectionY < NumChunks; ChunkSubsectionY++) {
		for(int ChunkSubsectionX = 0; ChunkSubsectionX < NumChunks; ChunkSubsectionX++) {


			// Basically a chunk ID for the loop
			int ChunkSubsection = ChunkSubsectionX + ChunkSubsectionY * NumChunks;

			//// Start of the creation of a static mesh
			//FMeshDescription MeshDescription;
			//FStaticMeshAttributes Attributes(MeshDescription);
			//Attributes.Register();

			//FMeshDescriptionBuilder MeshDescBuilder;
			//MeshDescBuilder.SetMeshDescription(&MeshDescription);
			//MeshDescBuilder.EnablePolyGroups();
			//MeshDescBuilder.SetNumUVLayers(1);

			//TArray<FVertexInstanceID> vertexInsts;
			////vertexInsts.SetNum(FTerrainInfo::ChunkSize * FTerrainInfo::ChunkSize);
			//FVertexInstanceID instance;

			//FVector4f RandomColor = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);

			//for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
			//	for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {
			//		instance = MeshDescBuilder.AppendInstance(MeshDescBuilder.AppendVertex(FVector(
			//			x * FTerrainInfo::QuadSize,
			//			y * FTerrainInfo::QuadSize,
			//			0.f)));
			//		MeshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
			//		MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 0);
			//		MeshDescBuilder.SetInstanceColor(instance, RandomColor);
			//		vertexInsts.Add(instance);
			//	}
			//}


			//// Allocate a polygon group
			//FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();


			//int RowCurrent, RowNext;
			//uint8 ChunkSize = FTerrainInfo::ChunkSize + 1;
			//// Add triangles to mesh description
			//for(int y = 0; y < FTerrainInfo::ChunkSize; ++y) {
			//	for(int x = 0; x < FTerrainInfo::ChunkSize; ++x) {
			//		RowCurrent = x + ChunkSize * y;
			//		RowNext = RowCurrent + ChunkSize;

			//		MeshDescBuilder.AppendTriangle(
			//			vertexInsts[RowCurrent],
			//			vertexInsts[RowNext],
			//			vertexInsts[RowCurrent + 1], PolygonGroup);


			//		MeshDescBuilder.AppendTriangle(
			//			vertexInsts[RowCurrent + 1],
			//			vertexInsts[RowNext],
			//			vertexInsts[RowNext + 1], PolygonGroup);



			//	}
			//}


			//// At least one material must be added
			//StaticMesh.Add(NewObject<UStaticMesh>(this));
			//StaticMesh[ChunkSubsection]->GetStaticMaterials().Add(FStaticMaterial());

			//UStaticMesh::FBuildMeshDescriptionsParams MDParams;
			//MDParams.bBuildSimpleCollision = true;
			//MDParams.bFastBuild = true;

			//// Build static mesh
			//TArray<const FMeshDescription*> MeshDescPtrs;

			//MeshDescPtrs.Emplace(&MeshDescription);
			//StaticMesh[ChunkSubsection]->BuildFromMeshDescriptions(MeshDescPtrs, MDParams);


			//// Create the UStaticMeshComponent, give its name, location and properties
			//FVector SectionLocalPos = FVector(
			//	SectionPos.X * SectionLocalDimension + ChunkSubsectionX * ChunkDimension,
			//	SectionPos.Y * SectionLocalDimension + ChunkSubsectionY * ChunkDimension,
			//	0.0f);

			FInt32Vector2 ChunkPos = FInt32Vector2(
				Chunk.X + ChunkSubsectionX,
				Chunk.Y + ChunkSubsectionY);

			//UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d done from Section %dx%d."), ChunkPos.X, ChunkPos.Y, SectionPos.X, SectionPos.Y);


			FGraphEventRef MyAsyncTask = FFunctionGraphTask::CreateAndDispatchWhenReady([ChunkPos, SectionPos, this]() {
				LoadChunkAsync(ChunkPos, SectionPos);
			}, TStatId(), nullptr, ENamedThreads::AnyBackgroundHiPriTask);
			FGraphEventArray MyTasks = { MyAsyncTask };
			FGraphEventRef MyTask = FFunctionGraphTask::CreateAndDispatchWhenReady([&]() {
				LoadDataFromAsyncLoad();
			}, TStatId(), &MyTasks, ENamedThreads::GameThread);

			//FString ChunkID = "Chunk ";
			//ChunkID.Append(FString::FromInt(FirstChunkInSection.X + ChunkSubsectionX));
			//ChunkID.Append("-");
			//ChunkID.Append(FString::FromInt(FirstChunkInSection.Y + ChunkSubsectionY));

			//// Assign new static mesh to the static mesh component
			//ChunkStaticMesh.Add(NewObject<UStaticMeshComponent>(this, FName(ChunkID)));
			//ChunkStaticMesh[ChunkSubsection]->SetStaticMesh(StaticMesh[ChunkSubsection]);
			//ChunkStaticMesh[ChunkSubsection]->SetWorldLocation(SectionLocalPos);
			//ChunkStaticMesh[ChunkSubsection]->Mobility = EComponentMobility::Static;
			//ChunkStaticMesh[ChunkSubsection]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			//ChunkStaticMesh[ChunkSubsection]->SetCollisionResponseToAllChannels(ECR_Block);
			//ChunkStaticMesh[ChunkSubsection]->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);


			//ChunkStaticMesh[ChunkSubsection]->RegisterComponent();


			//GetCluster()->AddInstanceComponent(ChunkStaticMesh[ChunkSubsection]);


		}
	}


}

void UTerrainSection::LoadChunkAsync(FInt32Vector2 Chunk, FInt32Vector2 Section) {

	// Basically a chunk ID for the loop
	int ChunkSubsection = Chunk.X + Chunk.Y * FTerrainInfo::SectionsPerCluster;

	// Start of the creation of a static mesh
	FMeshDescription MeshDescription;
	FStaticMeshAttributes Attributes(MeshDescription);
	Attributes.Register();

	FMeshDescriptionBuilder MeshDescBuilder;
	MeshDescBuilder.SetMeshDescription(&MeshDescription);
	MeshDescBuilder.EnablePolyGroups();
	MeshDescBuilder.SetNumUVLayers(1);

	TArray<FVertexInstanceID> vertexInsts;
	//vertexInsts.SetNum(FTerrainInfo::ChunkSize * FTerrainInfo::ChunkSize);
	FVertexInstanceID instance;

	FVector4f RandomColor = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);

	for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
		for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {
			instance = MeshDescBuilder.AppendInstance(MeshDescBuilder.AppendVertex(FVector(
				x * FTerrainInfo::QuadSize,
				y * FTerrainInfo::QuadSize,
				0.f)));
			MeshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
			MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 0);
			MeshDescBuilder.SetInstanceColor(instance, RandomColor);
			vertexInsts.Add(instance);
		}
	}


	// Allocate a polygon group
	FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();


	int RowCurrent, RowNext;
	uint8 ChunkSize = FTerrainInfo::ChunkSize + 1;
	// Add triangles to mesh description
	for(int y = 0; y < FTerrainInfo::ChunkSize; ++y) {
		for(int x = 0; x < FTerrainInfo::ChunkSize; ++x) {
			RowCurrent = x + ChunkSize * y;
			RowNext = RowCurrent + ChunkSize;

			MeshDescBuilder.AppendTriangle(
				vertexInsts[RowCurrent],
				vertexInsts[RowNext],
				vertexInsts[RowCurrent + 1], PolygonGroup);


			MeshDescBuilder.AppendTriangle(
				vertexInsts[RowCurrent + 1],
				vertexInsts[RowNext],
				vertexInsts[RowNext + 1], PolygonGroup);



		}
	}

	// At least one material must be added
	FChunkData load;
	//load.SectionPos = Section;
	load.ChunkPos = Chunk;
	load.Mesh = NewObject<UStaticMesh>();
	load.Mesh->GetStaticMaterials().Add(FStaticMaterial());

	UStaticMesh::FBuildMeshDescriptionsParams MDParams;
	MDParams.bBuildSimpleCollision = true;
	MDParams.bFastBuild = true;

	// Build static mesh
	TArray<const FMeshDescription*> MeshDescPtrs;

	MeshDescPtrs.Emplace(&MeshDescription);
	load.Mesh->BuildFromMeshDescriptions(MeshDescPtrs, MDParams);

	FScopeLock lock(&ChunkLoadLock);

	StaticMeshLoadAsync.Enqueue(load);

	//UE_LOG(LogTemp, Warning, TEXT("Chunk %dx%d Mesh loaded async."), Chunk.X, Chunk.Y);

}

void UTerrainSection::LoadDataFromAsyncLoad() {

	FScopeLock lock(&ChunkLoadLock);

	FChunkData load;
	StaticMeshLoadAsync.Dequeue(load);

	/*if(load == FAsyncChunkLoad())
		return;*/

	int SectionLocalDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize * FTerrainInfo::SectionsPerCluster;
	int ChunkDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize;

	// Create the UStaticMeshComponent, give its name, location and properties
	FVector SectionLocalPos = FVector(
		load.ChunkPos.X * ChunkDimension,
		load.ChunkPos.Y * ChunkDimension,
		0.0f);

	FString ChunkID = "Chunk ";
	ChunkID.Append(FString::FromInt(load.ChunkPos.X));
	ChunkID.Append("-");
	ChunkID.Append(FString::FromInt(load.ChunkPos.Y));

	// Assign new static mesh to the static mesh component


	load.ChunkComponent = NewObject<UStaticMeshComponent>(this, FName(ChunkID));
	int32 meshID = StaticMesh.Add(load);
	load.ChunkComponent->SetStaticMesh(load.Mesh);
	load.ChunkComponent->SetWorldLocation(SectionLocalPos);
	load.ChunkComponent->Mobility = EComponentMobility::Static;
	load.ChunkComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	load.ChunkComponent->SetCollisionResponseToAllChannels(ECR_Block);
	load.ChunkComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);


	load.ChunkComponent->RegisterComponent();


	GetCluster()->AddInstanceComponent(load.ChunkComponent);
}

void  UTerrainSection::HideOutOfRangeChunks(const FVector2D& SphereCenter, float SphereRadius) {
	//UE_LOG(LogTemp, Warning, TEXT("Called for section: %dx%d"), SectionBaseX, SectionBaseY);

	FVector2D SquareMin, SquareMax;

	for(int i = 0; i < StaticMesh.Num(); i++) {
		FInt32Vector2 base = StaticMesh[i].ChunkPos;
		SquareMin = FVector2D(base.X * FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize,
							  base.Y * FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize);
		SquareMax = FVector2D(SquareMin.X + FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize,
							  SquareMin.Y + FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize);
		//UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d"), base.X, base.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Start: %lfx%lf"), SquareMin.X, SquareMin.Y);
		//UE_LOG(LogTemp, Warning, TEXT("End:   %lfx%lf"), SquareMax.X, SquareMax.Y);


		// Calculate the closest point on the square/rectangle to the center of the sphere
		float ClosestX = FMath::Clamp(SphereCenter.X, SquareMin.X, SquareMax.X);
		float ClosestY = FMath::Clamp(SphereCenter.Y, SquareMin.Y, SquareMax.Y);
		FVector2D ClosestPoint(ClosestX, ClosestY);

		// Check if the distance between the closest point and the center of the sphere is less than or equal to the sphere radius
		float DistanceSquared = FVector2D::DistSquared(SphereCenter, ClosestPoint);
		float RadiusSquared = SphereRadius * SphereRadius;
		if(DistanceSquared <= RadiusSquared)
			StaticMesh[i].ChunkComponent->SetVisibility(true);
		else
			StaticMesh[i].ChunkComponent->SetVisibility(false); // This doesn't actually improve performance
	}

}

// Synch chunk loaded DEPRECATED
//void UTerrainSection::CreateSection(FInt32Vector2 SectorPos, FInt32Vector2 FirstChunkInSector, int32 numSubsections, int32 chunkSize, UTexture2D* heightMap) {
//
//	SectionBaseX = SectorPos.X;
//	SectionBaseY = SectorPos.Y;
//
//	NumChunks = numSubsections;
//	ChunkNumQuads = chunkSize - 1;
//	TotalNumQuads = ChunkNumQuads * (numSubsections ^ 2);
//
//	FGuid name = FGuid::NewGuid();
//	//ChunkMesh = NewObject<UProceduralMeshComponent>(this, FName(name.ToString()));
//	float SectionLocalDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize * FTerrainInfo::SectionsPerCluster;
//	float ChunkDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize;
//
//	//Static mesh version
//
//	for(int ChunkSubsectionY = 0; ChunkSubsectionY < numSubsections; ChunkSubsectionY++) {
//		for(int ChunkSubsectionX = 0; ChunkSubsectionX < numSubsections; ChunkSubsectionX++) {
//
//			// Basically a chunk ID for the loop
//			int ChunkSubsection = ChunkSubsectionX + ChunkSubsectionY * numSubsections;
//
//			// Start of the creation of a static mesh
//			FMeshDescription MeshDescription;
//			FStaticMeshAttributes Attributes(MeshDescription);
//			Attributes.Register();
//
//			FMeshDescriptionBuilder MeshDescBuilder;
//			MeshDescBuilder.SetMeshDescription(&MeshDescription);
//			MeshDescBuilder.EnablePolyGroups();
//			MeshDescBuilder.SetNumUVLayers(1);
//
//			TArray<FVertexInstanceID> vertexInsts;
//			//vertexInsts.SetNum(FTerrainInfo::ChunkSize * FTerrainInfo::ChunkSize);
//			FVertexInstanceID instance;
//
//			FVector4f RandomColor = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
//
//			for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
//				for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {
//					instance = MeshDescBuilder.AppendInstance(MeshDescBuilder.AppendVertex(FVector(
//						x * FTerrainInfo::QuadSize,
//						y * FTerrainInfo::QuadSize,
//						FMath::RandRange(0.0f, 50.0f))));
//					MeshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
//					MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 0);
//					MeshDescBuilder.SetInstanceColor(instance, RandomColor);
//					vertexInsts.Add(instance);
//				}
//			}
//
//			// Allocate a polygon group
//			FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();
//			FStaticMeshOperations::RecomputeNormalsAndTangentsIfNeeded(MeshDescription, EComputeNTBsFlags::Normals);
//
//			int RowCurrent, RowNext;
//			// Add triangles to mesh description
//			for(int y = 0; y < FTerrainInfo::ChunkSize; y++) {
//				for(int x = 0; x < FTerrainInfo::ChunkSize; x++) {
//					RowCurrent = x + (FTerrainInfo::ChunkSize + 1) * y;
//					RowNext = RowCurrent + FTerrainInfo::ChunkSize + 1;
//
//					FTriangleID triangle = MeshDescBuilder.AppendTriangle(
//						vertexInsts[RowCurrent],
//						vertexInsts[RowNext],
//						vertexInsts[RowCurrent + 1], PolygonGroup);
//
//
//					triangle = MeshDescBuilder.AppendTriangle(
//						vertexInsts[RowCurrent + 1],
//						vertexInsts[RowNext],
//						vertexInsts[RowNext + 1], PolygonGroup);
//
//
//
//				}
//			}
//
//			// At least one material must be added
//			StaticMesh.Add(NewObject<UStaticMesh>(this));
//			StaticMesh[ChunkSubsection]->GetStaticMaterials().Add(FStaticMaterial());
//
//			UStaticMesh::FBuildMeshDescriptionsParams MDParams;
//			MDParams.bBuildSimpleCollision = true;
//			MDParams.bFastBuild = true;
//
//			// Build static mesh
//			TArray<const FMeshDescription*> MeshDescPtrs;
//
//			MeshDescPtrs.Emplace(&MeshDescription);
//			StaticMesh[ChunkSubsection]->BuildFromMeshDescriptions(MeshDescPtrs, MDParams);
//
//			// Create the UStaticMeshComponent, give its name, location and properties
//			FVector SectionLocalPos = FVector(
//				SectorPos.X * SectionLocalDimension + ChunkSubsectionX * ChunkDimension,
//				SectorPos.Y * SectionLocalDimension + ChunkSubsectionY * ChunkDimension,
//				0.0f);
//
//			FString ChunkID = "Chunk ";
//			ChunkID.Append(FString::FromInt(FirstChunkInSector.X + ChunkSubsectionX));
//			ChunkID.Append("-");
//			ChunkID.Append(FString::FromInt(FirstChunkInSector.Y + ChunkSubsectionY));
//
//			// Assign new static mesh to the static mesh component
//			ChunkStaticMesh.Add(NewObject<UStaticMeshComponent>(this, FName(ChunkID)));
//			ChunkStaticMesh[ChunkSubsection]->SetStaticMesh(StaticMesh[ChunkSubsection]);
//			ChunkStaticMesh[ChunkSubsection]->SetWorldLocation(SectionLocalPos);
//			ChunkStaticMesh[ChunkSubsection]->Mobility = EComponentMobility::Static;
//			ChunkStaticMesh[ChunkSubsection]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//			ChunkStaticMesh[ChunkSubsection]->SetCollisionResponseToAllChannels(ECR_Block);
//			ChunkStaticMesh[ChunkSubsection]->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
//
//
//			ChunkStaticMesh[ChunkSubsection]->RegisterComponent();
//
//
//			GetCluster()->AddInstanceComponent(ChunkStaticMesh[ChunkSubsection]);
//
//		}
//	}
//
//}

void UTerrainSection::CreateChunkMeshFromHeightMap(FInt32Vector2 ChunkPos, const MArray<float>& HeightMap) {


	int ChunkPosition = FTerrainInfo::ChunkToSectionPosition(ChunkPos);
	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d at pos in sector: %d"), ChunkPos.X, ChunkPos.Y, ChunkPosition);

	//Load the reference to the main floot/chunk material.
	// Assuming you have a variable named "MaterialPath" containing the path of the material you want to load
	// (e.g. "/Game/MyFolder/MyMaterial.MyMaterial")
	//FSoftObjectPath MaterialReference("/Script/Engine.Material'/Game/_Game/Floor.Floor'");
	//UMaterialInterface* Material = Cast<UMaterialInterface>(MaterialReference.TryLoad());

	// Start of the creation of a static mesh
	FMeshDescription MeshDescription;
	FStaticMeshAttributes Attributes(MeshDescription);
	Attributes.Register();

	FMeshDescriptionBuilder MeshDescBuilder;
	MeshDescBuilder.SetMeshDescription(&MeshDescription);
	MeshDescBuilder.EnablePolyGroups();
	MeshDescBuilder.SetNumUVLayers(1);

	// Calculate the normals for the mesh
	// For this we need the vertices and triangles, but at this point we don't have them yet, and by the way the mesh is build,
	// I don't see a proper way to do during that procress, so we are going calculate the this data twice
	// A better method is welcomed

	// Vertices part
	TArray<FVector> TempVertices, Normals;
	TArray<FVector2D> TempUVs;
	TempVertices.Reserve((FTerrainInfo::ChunkSize + 1) * (FTerrainInfo::ChunkSize + 1));
	Normals.Reserve((FTerrainInfo::ChunkSize + 1) * (FTerrainInfo::ChunkSize + 1));
	TempUVs.Reserve((FTerrainInfo::ChunkSize + 1) * (FTerrainInfo::ChunkSize + 1));

	for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
		for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {

			// Vertex
			TempVertices.Add(FVector(
				x * FTerrainInfo::QuadSize, // We are doing the same operation
				y * FTerrainInfo::QuadSize, // and we avoid doing two multiplications
				HeightMap.getItem(x, y)));

			// Normal initialization, will be used to add weights later
			Normals.Add(FVector(0.0f));
			TempUVs.Add(FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)));
		}
	}

	// Triangles part
	TArray<FIntVector> TempTriangles;
	TempTriangles.Reserve(FTerrainInfo::ChunkSize * FTerrainInfo::ChunkSize * 2);

	int RowCurrentT, RowNextT;
	for(int y = 0; y < FTerrainInfo::ChunkSize; y++) {
		for(int x = 0; x < FTerrainInfo::ChunkSize; x++) {
			RowCurrentT = x + (FTerrainInfo::ChunkSize + 1) * y;
			RowNextT = RowCurrentT + FTerrainInfo::ChunkSize + 1;

			TempTriangles.Add(FIntVector(RowCurrentT,
										 RowNextT,
										 RowCurrentT + 1));

			TempTriangles.Add(FIntVector(RowCurrentT + 1,
										 RowNextT,
										 RowNextT + 1));
		}
	}



	// Calculate the normals
	for(int i = 0; i < TempTriangles.Num(); i++) {

		const int VertexA = TempTriangles[i].X; // We get the three vertex indices
		const int VertexB = TempTriangles[i].Y;
		const int VertexC = TempTriangles[i].Z;

		const FVector VectorAB = TempVertices[VertexA] - TempVertices[VertexB]; // We calculate two vector of the triangle
		const FVector VectorCB = TempVertices[VertexC] - TempVertices[VertexB];
		const FVector ResultNormal = FVector::CrossProduct(VectorAB, VectorCB); // Cross product of to vectors gives us the normals

		Normals[VertexA] += ResultNormal; //We add the weight to the involved vertex
		Normals[VertexB] += ResultNormal;
		Normals[VertexC] += ResultNormal;

		/*if(VertexA == FTerrainInfo::ChunkSize + 2 || VertexB == FTerrainInfo::ChunkSize + 2 || VertexC == FTerrainInfo::ChunkSize + 2) {
			UE_LOG(LogTemp, Warning, TEXT("Vertex: %d - %d - %d"), VertexA, VertexB, VertexC);
			UE_LOG(LogTemp, Warning, TEXT("VectorAB: %fx%fx%f"), VectorAB.X, VectorAB.Y, VectorAB.Z);
			UE_LOG(LogTemp, Warning, TEXT("VectorCB: %fx%fx%f"), VectorCB.X, VectorCB.Y, VectorCB.Z);
			UE_LOG(LogTemp, Warning, TEXT("ResultNormal: %fx%fx%f"), ResultNormal.X, ResultNormal.Y, ResultNormal.Z);
		}*/
	}

	//int NormalsTextPos = FTerrainInfo::ChunkSize + 2;
	//UE_LOG(LogTemp, Warning, TEXT("Normal: %fx%fx%f"), Normals[NormalsTextPos].X, Normals[NormalsTextPos].Y, Normals[NormalsTextPos].Z);

	for(int i = 0; i < Normals.Num(); i++) {
		Normals[i].Normalize();
	}

	//UE_LOG(LogTemp, Warning, TEXT("Normal: %fx%fx%f"), Normals[NormalsTextPos].X, Normals[NormalsTextPos].Y, Normals[NormalsTextPos].Z);

	// Let's try to solve Tangets
	int triangleCount = TempTriangles.Num();
	int vertexCount = TempVertices.Num();

	TArray<FVector> tan1, tan2, tangents;
	TArray<bool> sign;
	tan1.Init(FVector(0.0f), vertexCount);
	tan2.Init(FVector(0.0f), vertexCount);
	tangents.Reserve(vertexCount);
	sign.Reserve(vertexCount);

	for(long a = 0; a < triangleCount; a++) {

		int i1 = TempTriangles[a].X;
		int i2 = TempTriangles[a].Y;
		int i3 = TempTriangles[a].Z;
		FVector v1 = TempVertices[i1];
		FVector v2 = TempVertices[i2];
		FVector v3 = TempVertices[i3];

		FVector2D w1 = TempUVs[i1];
		FVector2D w2 = TempUVs[i2];
		FVector2D w3 = TempUVs[i3];

		float x1 = v2.X - v1.X;
		float x2 = v3.X - v1.X;
		float y1 = v2.Y - v1.Y;
		float y2 = v3.Y - v1.Y;
		float z1 = v2.Z - v1.Z;
		float z2 = v3.Z - v1.Z;

		float s1 = w2.X - w1.X;
		float s2 = w3.X - w1.X;
		float t1 = w2.Y - w1.Y;
		float t2 = w3.Y - w1.Y;

		float r = 1.0f / (s1 * t2 - s2 * t1);

		FVector sdir = FVector((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		FVector tdir = FVector((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for(long a = 0; a < vertexCount; ++a) {
		FVector n = Normals[a];
		FVector t = tan1[a];
		FVector tmp = (t - n * n.Dot(t));
		tmp.Normalize();

		tangents.Add(FVector(tmp.X, tmp.Y, tmp.Z));
		sign.Add(FVector::DotProduct(FVector::CrossProduct(n, t), tan2[a]) < 0.0f ? false : true);
	}


	TArray<FVertexInstanceID> vertexInsts;
	FVertexInstanceID instance;

	for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
		for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {
			/*instance = MeshDescBuilder.AppendInstance(MeshDescBuilder.AppendVertex(FVector(
				x * FTerrainInfo::QuadSize,
				y * FTerrainInfo::QuadSize,
				HeightMap.getItem(x, y))));*/

			instance = MeshDescBuilder.AppendInstance(
				MeshDescBuilder.AppendVertex(
					TempVertices[y * (FTerrainInfo::ChunkSize + 1) + x]
				)
			);

			//MeshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
			MeshDescBuilder.SetInstanceNormal(instance, Normals[y * (FTerrainInfo::ChunkSize + 1) + x]);
			MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 0);
			//MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 1);

			MeshDescBuilder.SetInstanceTangentSpace(instance,
													Normals[y * (FTerrainInfo::ChunkSize + 1) + x],
													tangents[y * (FTerrainInfo::ChunkSize + 1) + x],
													sign[y * (FTerrainInfo::ChunkSize + 1) + x]);
			//MeshDescBuilder.SetInstanceColor(instance, RandomColor);
			vertexInsts.Add(instance);

		}
	}

	// Allocate a polygon group
	FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();
	//FStaticMeshOperations::RecomputeNormalsAndTangentsIfNeeded(MeshDescription, EComputeNTBsFlags::Normals); //This do nothing here?
	//FStaticMeshOperations::ComputeTangentsAndNormals(MeshDescription, EComputeNTBsFlags::Normals);

	int RowCurrent, RowNext;
	// Add triangles to mesh description
	for(int y = 0; y < FTerrainInfo::ChunkSize; y++) {
		for(int x = 0; x < FTerrainInfo::ChunkSize; x++) {
			RowCurrent = x + (FTerrainInfo::ChunkSize + 1) * y;
			RowNext = RowCurrent + FTerrainInfo::ChunkSize + 1;

			FTriangleID triangle = MeshDescBuilder.AppendTriangle(
				vertexInsts[RowCurrent],
				vertexInsts[RowNext],
				vertexInsts[RowCurrent + 1], PolygonGroup);


			triangle = MeshDescBuilder.AppendTriangle(
				vertexInsts[RowCurrent + 1],
				vertexInsts[RowNext],
				vertexInsts[RowNext + 1], PolygonGroup);



		}
	}

	// At least one material must be added
	StaticMesh[ChunkPosition].Mesh = NewObject<UStaticMesh>(this);
	StaticMesh[ChunkPosition].Mesh->GetStaticMaterials().Add(FStaticMaterial());

	UStaticMesh::FBuildMeshDescriptionsParams MDParams;
	MDParams.bBuildSimpleCollision = true;
	MDParams.bFastBuild = true;

	// Build static mesh
	TArray<const FMeshDescription*> MeshDescPtrs;

	MeshDescPtrs.Emplace(&MeshDescription);
	StaticMesh[ChunkPosition].Mesh->BuildFromMeshDescriptions(MeshDescPtrs, MDParams);

	// Build the collision data for the mesh
	StaticMesh[ChunkPosition].Mesh->CreateBodySetup();
	StaticMesh[ChunkPosition].Mesh->ComplexCollisionMesh = StaticMesh[ChunkPosition].Mesh;
	StaticMesh[ChunkPosition].Mesh->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;

	StaticMesh[ChunkPosition].Mesh->GetBodySetup()->InvalidatePhysicsData();
	StaticMesh[ChunkPosition].Mesh->GetBodySetup()->CreatePhysicsMeshes();
	StaticMesh[ChunkPosition].Mesh->MarkPackageDirty();

	StaticMesh[ChunkPosition].ChunkComponent->SetStaticMesh(StaticMesh[ChunkPosition].Mesh);
	StaticMesh[ChunkPosition].ChunkComponent->UpdateCollisionFromStaticMesh();





	//ChunkStaticMesh[ChunkPosition]->updatecompone();

}

void UTerrainSection::CreateMaterialsFromBiomeMap(FInt32Vector2 ChunkPos, const MArray<uint8>& BiomeMap) {

	int ChunkPosition = FTerrainInfo::ChunkToSectionPosition(ChunkPos);
	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d at pos in sector: %d"), ChunkPos.X, ChunkPos.Y, ChunkPosition);

	//Load the reference to the main floot/chunk material.
	// Assuming you have a variable named "MaterialPath" containing the path of the material you want to load
	// (e.g. "/Game/MyFolder/MyMaterial.MyMaterial")
	FSoftObjectPath MaterialReference("/Script/Engine.Material'/Game/_Game/Floor.Floor'");
	UMaterialInterface* Material = Cast<UMaterialInterface>(MaterialReference.TryLoad());

	// Time to create the textures for our chunks, first, check if the root material is loaded properly
	if(!Material) {
		// Failed to load the material
		UE_LOG(LogTemp, Error, TEXT("Fail at loading the floor material!!!"));
	}

	// The material was successfully loaded, you can now use it
	// Assuming you have a variable named "Material" containing a reference to your material
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	if(DynMaterial) {
		// The dynamic material instance was successfully created, you can now change its parameters
		UE_LOG(LogTemp, Warning, TEXT("We got a dynamic material loaded."));

		// Set a scalar parameter value
		DynMaterial->SetScalarParameterValue("Blend", 0.5f);

		// Set a texture parameter value
		/*UTexture* TextureParameterValue = LoadObject<UTexture>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/_Game/931998321.931998321'"));
		DynMaterial->SetTextureParameterValue("TextureBase", TextureParameterValue);*/

		StaticMesh[ChunkPosition].ChunkComponent->SetOverlayMaterial(DynMaterial);
	}

}

/*
* Return the HeightMap of the give Chunk. The vertices are in local space.
*/
void UTerrainSection::GetChunkHeights(FInt32Vector2 ChunkPos, MArray<float>& HeightMap) {

	HeightMap = MArray<float>(FTerrainInfo::ChunkSize + 1, FTerrainInfo::ChunkSize + 1);

	int pos = FTerrainInfo::ChunkToSectionPosition(ChunkPos);
	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d at pos in sector: %d"), ChunkPos.X, ChunkPos.Y, pos);

	//if(!IsValidLowLevel()) return;
	if(!StaticMesh[pos].Mesh) return;
	if(!StaticMesh[pos].Mesh->GetRenderData()) return;

	// Get the vertex buffer from the static mesh
	FStaticMeshVertexBuffers& VertexBuffers = StaticMesh[pos].Mesh->GetRenderData()->LODResources[0].VertexBuffers;
	FPositionVertexBuffer* Vertices = &VertexBuffers.PositionVertexBuffer;

	if(!Vertices) {
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Vertices number %d"), Vertices->GetNumVertices());
	// Iterate over the vertices and print their positions
	for(uint32 index = 0; index < Vertices->GetNumVertices(); index++) {
		FVector3f vertex = Vertices->VertexPosition(index);
		//UE_LOG(LogTemp, Log, TEXT("Vertex %d: (%f, %f, %f)"), index, vertex.X, vertex.Y, vertex.Z);

		HeightMap.setItem(vertex.Z,
						  (int32)FMath::Fmod(index, (FTerrainInfo::ChunkSize + 1) * 1.0f),
						  index / (FTerrainInfo::ChunkSize + 1));

		/*if(index == 0 || index == (Vertices->GetNumVertices() - 1))
			UE_LOG(LogTemp, Log, TEXT("Index %d: (%f, %f, %f)"), index, vertex.X, vertex.Y, vertex.Z);*/


			//UE_LOG(LogTemp, Log, TEXT("Vertex %d: (%d, %d, %f)"), index, (int32) FMath::Fmod(index, (FTerrainInfo::ChunkSize + 1) * 1.0f), index / (FTerrainInfo::ChunkSize + 1), vertex.Z);

	}

}

ATerrainCluster* UTerrainSection::GetCluster() const {
	return CastChecked<ATerrainCluster>(GetOuter());
}

bool UTerrainSection::IsSectorLoaded(FInt32Vector2 section) {
	if(SectionBaseX == section.X)
		if(SectionBaseY == section.Y)
			return true;
	return false;
}