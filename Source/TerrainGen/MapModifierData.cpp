// Fill out your copyright notice in the Description page of Project Settings.



#include "MapModifierData.h"

void UMapModifierDataNoise::Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at noise."));

	MapHeights.PrintInfo();
	UE_LOG(LogTemp, Warning, TEXT("Map resolution: %d"), MapResolution);
	UE_LOG(LogTemp, Warning, TEXT("Biome Index: %d"), BiomeIndex);

	float WorkingXFrequency = BaseXFrequency;
	float WorkingYFrequency = BaseYFrequency;
	float WorkingNoiseAmount = NoiseAmount;

	for(int32 i = 0; i < octaves; i++) {
		for(int y = 0; y < MapResolution; y++) {
			for(int x = 0; x < MapResolution; x++) {
				// Skip if we have a biome and this is not our biome
				if(BiomeIndex >= 0 && BiomeMap.getItem(x, y) != BiomeIndex)
					continue;

				float PerlinValue = FMath::PerlinNoise2D(FVector2D(x * WorkingXFrequency, y * WorkingYFrequency));
				//UE_LOG(LogTemp, Warning, TEXT("PerlinValue %f"), PerlinValue);

				float NewHeight = MapHeights.getItem(x, y) + (PerlinValue * NoiseAmount * MapScale.Z);
				//UE_LOG(LogTemp, Warning, TEXT("NewHeight %f"), NewHeight);

				NewHeight = FMath::Lerp(MapHeights.getItem(x, y), NewHeight, Strength);


				//UE_LOG(LogTemp, Warning, TEXT("NewHeight %f at %dx%d"), NewHeight, x, y);

				// Blend based on strength
				MapHeights.setItem(NewHeight, x, y);
			}
		}

		WorkingXFrequency *= XFrequencyVariationPerOctane;
		WorkingYFrequency *= YFrequencyVariationPerOctane;
		WorkingNoiseAmount *= NoiseAmountVariationPerOctane;


	}

}

//void UMapModifierDataNoise::Execute(int MapResolution, MArray<float> &MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
//	UE_LOG(LogTemp, Warning, TEXT("We are at noise."));
//
//	MapHeights.PrintInfo();
//	UE_LOG(LogTemp, Warning, TEXT("Map resolution: %d"), MapResolution);
//	UE_LOG(LogTemp, Warning, TEXT("Biome Index: %d"), BiomeIndex);
//
//	for(int y = 0; y < MapResolution; y++) {
//		for(int x = 0; x < MapResolution; x++) {
//
//			// Skip if we have a biome and this is not our biome
//			if(BiomeIndex >= 0 && BiomeMap.getItem(x, y) != BiomeIndex)
//				continue;
//
//			float total = 0;
//			float frequency = 1;
//			float amplitude = 1.f;
//			float maxValue = 0;
//
//			for(int32 i = 0; i < octaves; i++) {
//
//				FVector2D result = FVector2D(x * BaseXFrequency * frequency, y * BaseYFrequency * frequency) * amplitude;
//				//UE_LOG(LogTemp, Warning, TEXT("FVector2D %lf x %lf"), result.X, result.Y);
//
//				total = FMath::PerlinNoise2D(result);
//				//UE_LOG(LogTemp, Warning, TEXT("Noise Value: %f"), total);
//
//				maxValue += amplitude;
//
//				//amplitude *= persistence;
//				frequency *= 2;
//			}
//
//			float NoiseValue = total / maxValue * NoiseAmount;
//
//			//UE_LOG(LogTemp, Warning, TEXT("Noise Value: %f"), NoiseValue);
//
//			// Calculate the new height
//			float newHeight = MapHeights.getItem(x, y) + (NoiseValue * NoiseAmount * MapScale.Z);
//			newHeight = FMath::Lerp(MapHeights.getItem(x, y), newHeight, Strength);
//
//			// Blend based on strength
//			MapHeights.setItem(newHeight, x, y);
//
//		}
//	}
//}

void UMapModifierDataRandom::Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at random."));

	MapHeights.PrintInfo();
	UE_LOG(LogTemp, Warning, TEXT("Map resolution: %d"), MapResolution);
	UE_LOG(LogTemp, Warning, TEXT("Biome Index: %d"), BiomeIndex);

	for(int y = 0; y < MapResolution; y++) {
		for(int x = 0; x < MapResolution; x++) {

			// Skip if we have a biome and this is not our biome
			if(BiomeIndex >= 0 && BiomeMap.getItem(x, y) != BiomeIndex)
				continue;

			// Calculate the new height
			float newHeight = MapHeights.getItem(x, y) + (FMath::RandRange(-HeightDelta, HeightDelta) * MapScale.Z);
			newHeight = FMath::Lerp(MapHeights.getItem(x, y), newHeight, Strength);

			// Blend based on strength
			MapHeights.setItem(newHeight, x, y);

		}
	}
}

void UMapModifierOffSet::Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at offset."));

	UE_LOG(LogTemp, Warning, TEXT("We are at offset."));

	MapHeights.PrintInfo();
	UE_LOG(LogTemp, Warning, TEXT("Map resolution: %d"), MapResolution);
	UE_LOG(LogTemp, Warning, TEXT("Biome Index: %d"), BiomeIndex);

	for(int y = 0; y < MapResolution; y++) {
		for(int x = 0; x < MapResolution; x++) {

			// Skip if we have a biome and this is not our biome
			if(BiomeIndex >= 0 && BiomeMap.getItem(x, y) != BiomeIndex)
				continue;

			// Calculate the new height
			float newHeight = MapHeights.getItem(x, y) + (OffsetAmount * MapScale.Z);
			newHeight = FMath::Lerp(MapHeights.getItem(x, y), newHeight, Strength);

			// Blend based on strength
			MapHeights.setItem(newHeight, x, y);

		}
	}
}

void UMapModifierSetValue::Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at setvalue."));

	MapHeights.PrintInfo();
	UE_LOG(LogTemp, Warning, TEXT("Map resolution: %d"), MapResolution);
	UE_LOG(LogTemp, Warning, TEXT("Biome Index: %d"), BiomeIndex);

	for(int y = 0; y < MapResolution; y++) {
		for(int x = 0; x < MapResolution; x++) {

			// Skip if we have a biome and this is not our biome
			if(BiomeIndex >= 0 && BiomeMap.getItem(x, y) != BiomeIndex)
				continue;

			// Calculate the new height
			float newHeight = TargetHeight * MapScale.Z;
			newHeight = FMath::Lerp(MapHeights.getItem(x, y), newHeight, Strength);

			// Blend based on strength
			MapHeights.setItem(newHeight, x, y);

		}
	}

}

void UMapModifierSmooth::Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at setvalue."));

	MapHeights.PrintInfo();
	UE_LOG(LogTemp, Warning, TEXT("Map resolution: %d"), MapResolution);
	UE_LOG(LogTemp, Warning, TEXT("Biome Index: %d"), BiomeIndex);

	if(BiomeIndex != -1) {
		UE_LOG(LogTemp, Error, TEXT(" UMapModifierSmooth is not supported as a per biome modifier. Yet."));
		return;
	}

	MArray<float> smoothedHeights = MArray<float>(0, MapResolution, MapResolution);


	for(int y = 0; y < MapResolution; y++) {
		for(int x = 0; x < MapResolution; x++) {

			float heightSum = 0.f;
			int numValues = 0;

			// Sum the neightbouring values
			for(int yDelta = -SmoothKernelSize; yDelta < SmoothKernelSize; ++yDelta) {

				int workingY = y + yDelta;
				if(workingY < 0 || workingY >= MapResolution)
					continue; // Out of bounds

				for(int xDelta = -SmoothKernelSize; xDelta < SmoothKernelSize; ++xDelta) {

					int workingX = x + xDelta;
					if(workingX < 0 || workingX >= MapResolution)
						continue; // Out of bounds

					heightSum += MapHeights.getItem(workingX, workingY);
					++numValues;
				}
			}

			// Store the smoothed (aka average) height
			smoothedHeights.setItem(heightSum / numValues, x, y);

		}
	}

	for(int y = 0; y < MapResolution; y++) {
		for(int x = 0; x < MapResolution; x++) {
			// Calculate the new height, Blend based on strength
			float newHeight = FMath::Lerp(MapHeights.getItem(x, y), smoothedHeights.getItem(x, y), Strength);

			MapHeights.setItem(newHeight, x, y);
		}
	}


	//for(int y = 0; y < MapResolution; y++) {
	//	for(int x = 0; x < MapResolution; x++) {

	//		// Skip if we have a biome and this is not our biome
	//		if(BiomeIndex >= 0 && BiomeMap.getItem(x, y) != BiomeIndex)
	//			continue;

	//		// Calculate the new height
	//		float newHeight = SmoothKernelSize * MapScale.Z;
	//		newHeight = FMath::Lerp(MapHeights.getItem(x, y), newHeight, Strength);

	//		// Blend based on strength
	//		MapHeights.setItem(newHeight, x, y);

	//	}
	//}

}

