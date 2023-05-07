// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTerrainGen.h"
#include <TerrainGen/Terrain.h>

//WorldTerrainGen::WorldTerrainGen() {}
//
//WorldTerrainGen::~WorldTerrainGen() {}

//FTerrainGenCurves WorldTerrainGen::PerlinNoiseGenCurves;

UTexture2D* WorldTerrainGen::GenerateClusterTexture(FIntPoint StartCluster, FIntPoint EndCluster, float BaseFrequency, const UCurveFloat* const Curve, int32 Octaves, float Persistence, float Frequency) {
	UE_LOG(LogTemp, Warning, TEXT("Continentalness Gen Start"));

	check(StartCluster.X < EndCluster.X || StartCluster.Y < EndCluster.Y)

	int ClusterRangeX = (EndCluster.X - StartCluster.X) + 1;
	int ClusterRangeY = (EndCluster.Y - StartCluster.Y) + 1;

	int Width = FTerrainInfo::ChunkSize * ClusterRangeX * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;
	int Height = FTerrainInfo::ChunkSize * ClusterRangeY * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;

	MArray<float> HeightMap(0.0f, Width, Height);

	for(int y = StartCluster.Y; y < ClusterRangeY; y++) {
		for(int x = StartCluster.X; x < ClusterRangeX; x++) {

			MArray<float> LocalHeightMap = ApplyCurveToPerlin(PerlinTerrainGen(FIntPoint(x, y), BaseFrequency), Curve);
			HeightMap.Append(LocalHeightMap,
							 x * FTerrainInfo::ChunkSize * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection,
							 y * FTerrainInfo::ChunkSize * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection);
		}
	}

	//MArray<float> HeightMap = ApplyCurveToPerlin(PerlinTerrainGen(StartCluster, BaseFrequency), Curve);

	/*int Width = HeightMap.GetArraySize().X;
	int Height = HeightMap.GetArraySize().Y;*/


	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8); //PF_B8G8R8A8
	FColor* FormatedImageData = static_cast<FColor*>(Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	//Perlin noise return values between -1 and 1, so in our curves, that is the working range
	float GetCurveMinValue = Curve->GetFloatValue(-1);
	float GetCurveMaxValue = Curve->GetFloatValue(1);



	for(int32 Y = 0; Y < Height; ++Y) {
		for(int32 X = 0; X < Width; ++X) {
			int32 PixelIndex = X + Y * Width;
			float ColorFactor = (HeightMap.getItem(X, Y) - GetCurveMinValue) / (GetCurveMaxValue - GetCurveMinValue);
			FColor PixelColor = FColor(255 * ColorFactor, 255 * ColorFactor, 255 * ColorFactor, 255);
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

MArray<float> WorldTerrainGen::GetClusterHeights(FIntPoint Cluster) {


	return ApplyCurveToPerlin(PerlinTerrainGen(Cluster, 1 / 256.f), PerlinNoiseGenCurves.ContinentalnessCurve);
	//MArray<float> HeightMap;

	//return PerlinTerrainGen(Cluster);
}

void WorldTerrainGen::InitializeCurves(FTerrainGenCurves Curves) {
	WorldTerrainGen::PerlinNoiseGenCurves = Curves;
}

MArray<float> WorldTerrainGen::PerlinTerrainGen(FIntPoint Cluster, float BaseFrequency, int32 Octaves, float Persistence, float Frequency) {
	int Width = FTerrainInfo::ChunkSize * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;
	int Height = FTerrainInfo::ChunkSize * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;

	MArray<float> HeightMap(0.0f, Width, Height);

	float FrequecyOffSet = 1 / BaseFrequency / FTerrainInfo::ChunkSize;

	FVector2D ClusterOffset = FVector2D(Cluster.X * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection / FrequecyOffSet,
										Cluster.Y * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection / FrequecyOffSet);

	FVector2D SamplePosTest = ClusterOffset + (FVector2D(0.5f, 0.5f) * BaseFrequency);
	//UE_LOG(LogTemp, Warning, TEXT("First sample pos: %fx%f"), SamplePosTest.X, SamplePosTest.Y);

	for(int32 Y = 0; Y < Height; ++Y) {
		for(int32 X = 0; X < Width; ++X) {
			float PerlinValue = 0.f;
			float Amplitude = 1.f;
			float FrequencyScale = BaseFrequency;

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

MArray<float> WorldTerrainGen::ApplyCurveToPerlin(MArray<float> PerlinNoise, const UCurveFloat* const Curve) {
	if(Curve == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Curve is null"));
		return PerlinNoise;
	}

	MArray<float> HeightMapCurved(0.0f, PerlinNoise.GetArraySize().X, PerlinNoise.GetArraySize().Y);
	for(uint32 j = 0; j < PerlinNoise.GetArraySize().Y; j++) {
		for(uint32 i = 0; i < PerlinNoise.GetArraySize().X; i++) {
			float PerlinValue = PerlinNoise.getItem(i, j);
			HeightMapCurved.setItem(Curve->GetFloatValue(PerlinValue), i, j);
		}
	}

	return HeightMapCurved;
}

