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

};
