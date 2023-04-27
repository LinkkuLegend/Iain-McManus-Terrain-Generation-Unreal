// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTerrainGen.h"
#include <TerrainGen/Terrain.h>

//WorldTerrainGen::WorldTerrainGen() {}
//
//WorldTerrainGen::~WorldTerrainGen() {}

UTexture2D* WorldTerrainGen::GenerateClusterTexture(FIntPoint Cluster, int32 Octaves, float Persistence, float Frequency) {
	UE_LOG(LogTemp, Warning, TEXT("Continentalness Gen Start"));

	MArray<float> HeightMap = PerlinTerrainGen(Cluster, Octaves, Persistence, Frequency);

	int Width = HeightMap.GetArraySize().X;
	int Height = HeightMap.GetArraySize().Y;

	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8); //PF_B8G8R8A8
	FColor* FormatedImageData = static_cast<FColor*>(Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));



	for(int32 Y = 0; Y < Height; ++Y) {
		for(int32 X = 0; X < Width; ++X) {
			int32 PixelIndex = X + Y * Width;
			float ColorFactor = (HeightMap.getItem(X, Y) + 1) / 2; //This is because perlin noise is between [-1,1], this transform it to [0,1]
			FColor PixelColor = FColor(255 * ColorFactor, 255 * ColorFactor, 255 * ColorFactor, 255 * ColorFactor);
			//FColor PixelColor = FColor(ColorFactor, ColorFactor, ColorFactor, 255);
			FormatedImageData[PixelIndex] = PixelColor;
		}
	}


	//Texture->AddToRoot();
	//Texture->Filter = TF_Nearest;
	//Texture->SRGB = false;
	//Texture->UpdateResource();

	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	const SIZE_T BytesTex = Texture->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);
	//const FString SizeStringTex = BytesToString(&BytesTex, 8);

	const SIZE_T BytesMap = HeightMap.GetResourceSizeBytes();
	//const FString SizeStringMap = BytesToString(&BytesMap, 8);



	UE_LOG(LogTemp, Warning, TEXT("Texture takes: %d"), BytesTex);
	UE_LOG(LogTemp, Warning, TEXT("Array allocated takes: %d"), BytesMap);

	return Texture;

}

MArray<float> WorldTerrainGen::PerlinTerrainGen(FIntPoint Cluster, int32 Octaves, float Persistence, float Frequency) {
	int Width = FTerrainInfo::ChunkSize * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;
	int Height = FTerrainInfo::ChunkSize * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;

	MArray<float> HeightMap(0.0f, Width, Height);

	FVector2D ClusterOffset = FVector2D(Cluster.X * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection,
										Cluster.Y * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection);


	for(int32 Y = 0; Y < Height; ++Y) {
		for(int32 X = 0; X < Width; ++X) {
			float PerlinValue = 0.f;
			float Amplitude = 1.f;
			float FrequencyScale = 1 / 64.f;

			FVector2D OctaveOffset = FVector2D(X + .5f, Y + .5f);

			for(int32 Octave = 0; Octave < Octaves; ++Octave) {

				FVector2D SamplePos = ClusterOffset + (OctaveOffset * FrequencyScale);
				PerlinValue += FMath::PerlinNoise2D(SamplePos) * Amplitude;
				Amplitude *= Persistence;
				FrequencyScale *= Frequency;
			}

			HeightMap.setItem(PerlinValue, X, Y);
		}
	}

	return HeightMap;
}

