// Fill out your copyright notice in the Description page of Project Settings.



#include "MapModifierData.h"


void UMapModifierDataNoise::Execute(int MapResolution, MArray<float> MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at noise."));
}

void UMapModifierDataRandom::Execute(int MapResolution, MArray<float> MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at random."));
}

void UMapModifierOffSet::Execute(int MapResolution, MArray<float> MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at offset."));
}

void UMapModifierSetValue::Execute(int MapResolution, MArray<float> MapHeights, FVector MapScale, MArray<uint8> BiomeMap, int BiomeIndex, UDA_BiomeConfig* BiomeConfig) {
	UE_LOG(LogTemp, Warning, TEXT("We are at setvalue."));

	/*for(int y = 0; y < MapResolution; y++) {
		for(int x = 0; x < MapResolution; x++) {

			MapHeights.setItem(TargetHeight * MapScale.Y , x , y);

		}
	}*/

}

