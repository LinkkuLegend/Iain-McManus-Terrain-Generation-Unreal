// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainSection.h"
#include "TerrainCluster.h"
#include "Terrain.h"

#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshOperations.h"
#include "StaticMeshAttributes.h"
#include <Runtime/MeshConversion/Public/MeshDescriptionBuilder.h>



void UTerrainSection::CreateSection(FInt32Vector2 SectorPos, FInt32Vector2 FirstChunkInSector, int32 numSubsections, int32 chunkSize, UTexture2D* heightMap) {

	SectionBaseX = SectorPos.X;
	SectionBaseY = SectorPos.Y;

	NumChunks = numSubsections;
	ChunkNumQuads = chunkSize - 1;
	TotalNumQuads = ChunkNumQuads * (numSubsections ^ 2);

	FGuid name = FGuid::NewGuid();
	//ChunkMesh = NewObject<UProceduralMeshComponent>(this, FName(name.ToString()));
	float SectionLocalDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize * FTerrainInfo::SectionsPerCluster;
	float ChunkDimension = FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize;

	//Procedural Mesh version

	/*for(int ChunkSubsectionY = 0; ChunkSubsectionY < numSubsections; ChunkSubsectionY++) {
		for(int ChunkSubsectionX = 0; ChunkSubsectionX < numSubsections; ChunkSubsectionX++) {

			int ChunkSubsection = ChunkSubsectionX + ChunkSubsectionY * numSubsections;

			FVector SectionLocalPos = FVector(
				SectorPos.X * SectionLocalDimension + ChunkSubsectionX * ChunkDimension,
				SectorPos.Y * SectionLocalDimension + ChunkSubsectionY * ChunkDimension,
				0.0f);

			FString ChunkID = "Chunk ";
			ChunkID.Append(FString::FromInt(FirstChunkInSector.X + ChunkSubsectionX));
			ChunkID.Append("-");
			ChunkID.Append(FString::FromInt(FirstChunkInSector.Y + ChunkSubsectionY));

			ChunkMesh.Add(NewObject<UProceduralMeshComponent>(this, FName(ChunkID)));
			ChunkMesh[ChunkSubsection]->SetWorldLocation(SectionLocalPos);
			ChunkMesh[ChunkSubsection]->Mobility = EComponentMobility::Static;

			TArray<FVector> Vertices, Normals;
			TArray<int> Triangles;
			TArray<FVector2D> UV0;

			for(int y = 0; y < FTerrainInfo::ChunkSize + 1; y++) {
				for(int x = 0; x < FTerrainInfo::ChunkSize + 1; x++) {
					Vertices.Add(FVector(x * FTerrainInfo::QuadSize, y * FTerrainInfo::QuadSize, 0));
					Normals.Add(FVector(0,0,1));
					UV0.Add(FVector2D(x/(FTerrainInfo::ChunkSize * 1.0f),y / (FTerrainInfo::ChunkSize * 1.0f)));
				}
			}

			//Debug stuff
			Vertices[0] = FVector(0 * FTerrainInfo::QuadSize, 0 * FTerrainInfo::QuadSize, 500);
			Vertices[((FTerrainInfo::ChunkSize + 1) * (FTerrainInfo::ChunkSize + 1)) - 1] = FVector(FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize, FTerrainInfo::ChunkSize * FTerrainInfo::QuadSize, 500);

			UE_LOG(LogTemp, Warning, TEXT("Num: %d"), Vertices.Num());
			UE_LOG(LogTemp, Warning, TEXT("Ours: %d"), FTerrainInfo::ChunkSize * FTerrainInfo::ChunkSize);



			int RowCurrent, RowNext;
			for(int y = 0; y < FTerrainInfo::ChunkSize; y++) {
				for(int x = 0; x < FTerrainInfo::ChunkSize; x++) {
					RowCurrent = x + (FTerrainInfo::ChunkSize + 1) * y;
					RowNext = RowCurrent + FTerrainInfo::ChunkSize + 1;
					Triangles.Add(RowCurrent);
					Triangles.Add(RowNext);
					Triangles.Add(RowCurrent + 1);
					Triangles.Add(RowCurrent + 1);
					Triangles.Add(RowNext);
					Triangles.Add(RowNext + 1);
				}
			}


			ChunkMesh[ChunkSubsection]->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
			ChunkMesh[ChunkSubsection]->RegisterComponent();
			GetCluster()->AddInstanceComponent(ChunkMesh[ChunkSubsection]);

		}
	}
	*/


	//Static mesh version

	for(int ChunkSubsectionY = 0; ChunkSubsectionY < numSubsections; ChunkSubsectionY++) {
		for(int ChunkSubsectionX = 0; ChunkSubsectionX < numSubsections; ChunkSubsectionX++) {

			// Basically a chunk ID for the loop
			int ChunkSubsection = ChunkSubsectionX + ChunkSubsectionY * numSubsections;

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
						FMath::RandRange(0.0f, 50.0f))));
					MeshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
					MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 0);
					MeshDescBuilder.SetInstanceColor(instance, RandomColor);
					vertexInsts.Add(instance);
				}
			}

			// Allocate a polygon group
			FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();
			FStaticMeshOperations::RecomputeNormalsAndTangentsIfNeeded(MeshDescription, EComputeNTBsFlags::Normals);

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
			StaticMesh.Add(NewObject<UStaticMesh>(this));
			StaticMesh[ChunkSubsection]->GetStaticMaterials().Add(FStaticMaterial());

			UStaticMesh::FBuildMeshDescriptionsParams MDParams;
			MDParams.bBuildSimpleCollision = true;
			MDParams.bFastBuild = true;

			// Build static mesh
			TArray<const FMeshDescription*> MeshDescPtrs;

			MeshDescPtrs.Emplace(&MeshDescription);
			StaticMesh[ChunkSubsection]->BuildFromMeshDescriptions(MeshDescPtrs, MDParams);

			// Create the UStaticMeshComponent, give its name, location and properties
			FVector SectionLocalPos = FVector(
				SectorPos.X * SectionLocalDimension + ChunkSubsectionX * ChunkDimension,
				SectorPos.Y * SectionLocalDimension + ChunkSubsectionY * ChunkDimension,
				0.0f);

			FString ChunkID = "Chunk ";
			ChunkID.Append(FString::FromInt(FirstChunkInSector.X + ChunkSubsectionX));
			ChunkID.Append("-");
			ChunkID.Append(FString::FromInt(FirstChunkInSector.Y + ChunkSubsectionY));

			// Assign new static mesh to the static mesh component
			ChunkStaticMesh.Add(NewObject<UStaticMeshComponent>(this, FName(ChunkID)));
			ChunkStaticMesh[ChunkSubsection]->SetStaticMesh(StaticMesh[ChunkSubsection]);
			ChunkStaticMesh[ChunkSubsection]->SetWorldLocation(SectionLocalPos);
			ChunkStaticMesh[ChunkSubsection]->Mobility = EComponentMobility::Static;
			ChunkStaticMesh[ChunkSubsection]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ChunkStaticMesh[ChunkSubsection]->SetCollisionResponseToAllChannels(ECR_Block);
			ChunkStaticMesh[ChunkSubsection]->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);


			ChunkStaticMesh[ChunkSubsection]->RegisterComponent();


			GetCluster()->AddInstanceComponent(ChunkStaticMesh[ChunkSubsection]);

		}
	}

}

void UTerrainSection::CreateChunkMeshFromHeightMap(FInt32Vector2 ChunkPos, const MArray<float>& HeightMap) {


	int ChunkPosition = FTerrainInfo::ChunkToSectionPosition(ChunkPos);
	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d at pos in sector: %d"), ChunkPos.X, ChunkPos.Y, ChunkPosition);

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
				HeightMap.getItem(x,y))));
			MeshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
			MeshDescBuilder.SetInstanceUV(instance, FVector2D(x / (FTerrainInfo::ChunkSize * 1.0f), y / (FTerrainInfo::ChunkSize * 1.0f)), 0);
			MeshDescBuilder.SetInstanceColor(instance, RandomColor);
			vertexInsts.Add(instance);
		}
	}

	// Allocate a polygon group
	FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();
	FStaticMeshOperations::RecomputeNormalsAndTangentsIfNeeded(MeshDescription, EComputeNTBsFlags::Normals);

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
	StaticMesh[ChunkPosition] = NewObject<UStaticMesh>(this);
	StaticMesh[ChunkPosition]->GetStaticMaterials().Add(FStaticMaterial());


	UStaticMesh::FBuildMeshDescriptionsParams MDParams;
	MDParams.bBuildSimpleCollision = true;
	MDParams.bFastBuild = true;

	// Build static mesh
	TArray<const FMeshDescription*> MeshDescPtrs;

	MeshDescPtrs.Emplace(&MeshDescription);
	StaticMesh[ChunkPosition]->BuildFromMeshDescriptions(MeshDescPtrs, MDParams);
	
	
	// Build the collision data for the mesh
	StaticMesh[ChunkPosition]->CreateBodySetup();
	//StaticMesh[ChunkPosition]->GetBodySetup()->AggGeom.ConvexElems.Empty();
	StaticMesh[ChunkPosition]->ComplexCollisionMesh = StaticMesh[ChunkPosition];
	StaticMesh[ChunkPosition]->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	/*StaticMesh[ChunkPosition]->GetBodySetup()->InvalidatePhysicsData();
	StaticMesh[ChunkPosition]->GetBodySetup()->CreatePhysicsMeshes();

	StaticMesh[ChunkPosition]->MarkPackageDirty();*/

	ChunkStaticMesh[ChunkPosition]->SetStaticMesh(StaticMesh[ChunkPosition]);
	ChunkStaticMesh[ChunkPosition]->bDrawMeshCollisionIfComplex = true;
	ChunkStaticMesh[ChunkPosition]->bDrawMeshCollisionIfSimple = true;
	ChunkStaticMesh[ChunkPosition]->UpdateCollisionFromStaticMesh();
	/*ChunkStaticMesh[ChunkPosition]->SetWorldLocation(SectionLocalPos);
	ChunkStaticMesh[ChunkPosition]->Mobility = EComponentMobility::Static;
	ChunkStaticMesh[ChunkPosition]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ChunkStaticMesh[ChunkPosition]->SetCollisionResponseToAllChannels(ECR_Block);
	ChunkStaticMesh[ChunkPosition]->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);*/


	//ChunkStaticMesh[ChunkPosition]->updatecompone();

}

/*
* Return the HeightMap of the give Chunk. The vertices are in local space.
*/
void UTerrainSection::GetChunkHeights(FInt32Vector2 ChunkPos, MArray<float>& HeightMap) {

	HeightMap = MArray<float>(FTerrainInfo::ChunkSize + 1, FTerrainInfo::ChunkSize + 1);

	int pos = FTerrainInfo::ChunkToSectionPosition(ChunkPos);
	UE_LOG(LogTemp, Warning, TEXT("Chunk: %dx%d at pos in sector: %d"), ChunkPos.X, ChunkPos.Y, pos);

	//if(!IsValidLowLevel()) return;
	if(!StaticMesh[pos]) return;
	if(!StaticMesh[pos]->GetRenderData()) return;

	// Get the vertex buffer from the static mesh
	FStaticMeshVertexBuffers& VertexBuffers = StaticMesh[pos]->GetRenderData()->LODResources[0].VertexBuffers;
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
						  (int32) FMath::Fmod(index, (FTerrainInfo::ChunkSize + 1) * 1.0f),
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