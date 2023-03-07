// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_BiomeConfig.h"
#include "DA_ProcGenConfig.generated.h"

USTRUCT(BlueprintType)
struct TERRAINGEN_API FBiomeConfig {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDA_BiomeConfig* Biome;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Weighting = 1.f;

};

/*UENUM()
enum class E_BiomeMapResolution : uint8 {
	Size_64x64		UMETA(DisplayName = "64"),
	Size_128x128	UMETA(DisplayName = "128"),
	Size_256x256	UMETA(DisplayName = "256"),
	Size_512x512	UMETA(DisplayName = "512")
};*/

UENUM()
enum class EBiomeMapResolution : uint8 {
	Size_64x64 = 1,
	Size_128x128 = 2,
	Size_256x256 = 4,
	Size_512x512 = 8
};



/**
 *
 */
UCLASS(BlueprintType)
class TERRAINGEN_API UDA_ProcGenConfig : public UDataAsset {
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FBiomeConfig> Biomes;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float BiomeSeedPointDensity = 0.1f;

	float GetTotalWeighting(){
		float sum = 0.f;
		for(auto& Biome : Biomes) 
			sum += Biome.Weighting;
		return sum;
	}

	int GetNumBiomes() {
		return Biomes.Num();
	}

	FORCEINLINE const uint16 GetBiomeMapResolution() {
		return (uint16)BiomeMapResolution * 64;
	}

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EBiomeMapResolution BiomeMapResolution = EBiomeMapResolution::Size_256x256;

};
