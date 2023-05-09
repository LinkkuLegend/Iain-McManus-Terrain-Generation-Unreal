// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTerrainGen.h"
#include <TerrainGen/Terrain.h>

//WorldTerrainGen::WorldTerrainGen() {}
//
//WorldTerrainGen::~WorldTerrainGen() {}

//FTerrainGenCurves WorldTerrainGen::PerlinNoiseGenCurves;

UTexture2D* WorldTerrainGen::GenerateClusterTexture(FIntPoint StartCluster, FIntPoint EndCluster,uint8 Maptype, float BaseFrequency, const UCurveFloat* const Curve, int32 Octaves, float Persistence, float Frequency) {
	UE_LOG(LogTemp, Warning, TEXT("Continentalness Gen Start"));

	check(StartCluster.X < EndCluster.X || StartCluster.Y < EndCluster.Y)

		int ClusterRangeX = (EndCluster.X - StartCluster.X) + 1;
	int ClusterRangeY = (EndCluster.Y - StartCluster.Y) + 1;

	int Width = FTerrainInfo::ChunkSize * ClusterRangeX * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;
	int Height = FTerrainInfo::ChunkSize * ClusterRangeY * FTerrainInfo::SectionsPerCluster * FTerrainInfo::ChunksPerSection + 1;

	MArray<float> HeightMap(0.0f, Width, Height);

	for(int y = StartCluster.Y; y < ClusterRangeY; y++) {
		for(int x = StartCluster.X; x < ClusterRangeX; x++) {

			MArray<float> LocalHeightMap = ApplyCurveToPerlin(PerlinTerrainGen(FIntPoint(x, y), Maptype, BaseFrequency, Octaves, Persistence, Frequency), Curve);
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
	/*float GetCurveMinValue = Curve->GetFloatValue(-1);
	float GetCurveMaxValue = Curve->GetFloatValue(1);*/

	float GetCurveMaxValue = -std::numeric_limits<float>::infinity();
	float GetCurveMinValue = std::numeric_limits<float>::infinity();
	for(const auto& Key : Curve->FloatCurve.GetConstRefOfKeys()) {
		GetCurveMaxValue = FMath::Max(GetCurveMaxValue, Key.Value);
		GetCurveMinValue = FMath::Min(GetCurveMinValue, Key.Value);
	}



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


	return ApplyCurveToPerlin(PerlinTerrainGen(Cluster,0, 1 / 2048.f, 7), PerlinNoiseGenCurves.ContinentalnessCurve);
	//MArray<float> HeightMap;

	//return PerlinTerrainGen(Cluster);
}

void WorldTerrainGen::Initialize(FTerrainGenCurves Curves, int32 SeedLocal) {
	Seed = SeedLocal;
	WorldTerrainGen::PerlinNoiseGenCurves = Curves;
}

MArray<float> WorldTerrainGen::PerlinTerrainGen(FIntPoint Cluster, uint8 MapType, float BaseFrequency, int32 Octaves, float Persistence, float Frequency) {
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

			FVector2D OctaveOffset = FVector2D(X + .5f, Y + .5f);
			FVector2D SamplePos = ClusterOffset + (OctaveOffset * BaseFrequency);

			for(int32 Octave = 0; Octave < Octaves; ++Octave) {

				SamplePos *= Frequency;
				PerlinValue += PerlinNoise2D(SamplePos, MapType) * Amplitude;
				Amplitude *= Persistence;
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

// Implementation of 1D, 2D and 3D Perlin noise based on Ken Perlin's improved version https://mrl.nyu.edu/~perlin/noise/
// (See Random3.tps for additional third party software info.)
namespace FMathPerlinHelpers
{
	// random permutation of 256 numbers, repeated 2x
	static int32 Permutation[5][512];

	// Gradient functions for 1D, 2D and 3D Perlin noise

	FORCEINLINE float Grad1(int32 Hash, float X) {
		// Slicing Perlin's 3D improved noise would give us only scales of -1, 0 and 1; this looks pretty bad so let's use a different sampling
		static const float Grad1Scales[16] = { -8 / 8, -7 / 8., -6 / 8., -5 / 8., -4 / 8., -3 / 8., -2 / 8., -1 / 8., 1 / 8., 2 / 8., 3 / 8., 4 / 8., 5 / 8., 6 / 8., 7 / 8., 8 / 8 };
		return Grad1Scales[Hash & 15] * X;
	}

	// Note: If you change the Grad2 or Grad3 functions, check that you don't change the range of the resulting noise as well; it should be (within floating point error) in the range of (-1, 1)
	FORCEINLINE float Grad2(int32 Hash, float X, float Y) {
		// corners and major axes (similar to the z=0 projection of the cube-edge-midpoint sampling from improved Perlin noise)
		switch(Hash & 7) {
			case 0: return X;
			case 1: return X + Y;
			case 2: return Y;
			case 3: return -X + Y;
			case 4: return -X;
			case 5: return -X - Y;
			case 6: return -Y;
			case 7: return X - Y;
				// can't happen
			default: return 0;
		}
	}

	FORCEINLINE float Grad3(int32 Hash, float X, float Y, float Z) {
		switch(Hash & 15) {
			// 12 cube midpoints
			case 0: return X + Z;
			case 1: return X + Y;
			case 2: return Y + Z;
			case 3: return -X + Y;
			case 4: return -X + Z;
			case 5: return -X - Y;
			case 6: return -Y + Z;
			case 7: return X - Y;
			case 8: return X - Z;
			case 9: return Y - Z;
			case 10: return -X - Z;
			case 11: return -Y - Z;
				// 4 vertices of regular tetrahedron
			case 12: return X + Y;
			case 13: return -X + Y;
			case 14: return -Y + Z;
			case 15: return -Y - Z;
				// can't happen
			default: return 0;
		}
	}

	// Curve w/ second derivative vanishing at 0 and 1, from Perlin's improved noise paper
	FORCEINLINE float SmoothCurve(float X) {
		return X * X * X * (X * (X * 6.0f - 15.0f) + 10.0f);
	}
};

float WorldTerrainGen::PerlinNoise2D(const FVector2D& Location, uint8 Permute) {
	using namespace FMathPerlinHelpers;

	float Xfl = FMath::FloorToFloat((float)Location.X);		// LWC_TODO: Precision loss
	float Yfl = FMath::FloorToFloat((float)Location.Y);
	int32 Xi = (int32)(Xfl) & 255;
	int32 Yi = (int32)(Yfl) & 255;
	float X = (float)Location.X - Xfl;
	float Y = (float)Location.Y - Yfl;
	float Xm1 = X - 1.0f;
	float Ym1 = Y - 1.0f;

	const int32* P = Permutation[Permute];
	int32 AA = P[Xi] + Yi;
	int32 AB = AA + 1;
	int32 BA = P[Xi + 1] + Yi;
	int32 BB = BA + 1;

	float U = SmoothCurve(X);
	float V = SmoothCurve(Y);

	// Note: Due to the choice of Grad2, this will be in the (-1,1) range with no additional scaling
	return FMath::Lerp(
		FMath::Lerp(Grad2(P[AA], X, Y), Grad2(P[BA], Xm1, Y), U),
		FMath::Lerp(Grad2(P[AB], X, Ym1), Grad2(P[BB], Xm1, Ym1), U),
		V);
}

void WorldTerrainGen::PerlinReset() {
	//We need to rebuild the permutation Array to get a new Perlin Noise "seed". The permutation is the "seed" for the Perlin Noise, and it is calculate by the current SRand seed
	//The permutatuon is an array of random numbers from 0 to 255, then copied again at the end. 512 numbers in total;

	using namespace FMathPerlinHelpers;
	FRandomStream RandomStream(Seed);
	uint8 NumberOfNoisesNeeded = 5;

	for(int x = 0; x < NumberOfNoisesNeeded; x++) {
		//We are gonna make the first part, so just fill an Array with the numbers from 0 to 512
		TArray<int32> numbers;
		numbers.SetNumUninitialized(256);
		for(int32 i = 0; i < 256; ++i)
			numbers[i] = i;

		//Then we shuffle the array
		int32 LastIndex = numbers.Num() - 1;
		for(int32 i = 0; i <= LastIndex; ++i) {
			int32 Index = RandomStream.RandRange(0, LastIndex);
			numbers.Swap(i, Index);
		}
		// Now we copy the array twice to the permutation
		for(int32 i = 0; i < 256; ++i) {
			Permutation[x][i] = numbers[i];
			Permutation[x][i + 256] = numbers[i];
		}
	}
};

