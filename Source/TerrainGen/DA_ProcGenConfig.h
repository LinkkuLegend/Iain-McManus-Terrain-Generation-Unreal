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

UENUM()
enum class EBiomeMapResolution : uint8 {
	Size_64x64 = 1,
	Size_128x128 = 2,
	Size_256x256 = 4,
	Size_512x512 = 8
};

UENUM()
enum class EMapModifierType : uint8 {
	Noise = 1,
	Random = 2,
	SetValue = 3,
	Offset = 4
};

UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class TERRAINGEN_API UMapModifierData : public UObject {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		EMapModifierType ModifierType = EMapModifierType::Noise;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Strength = 1.f;

	//UFUNCTION()
	virtual void Execute() PURE_VIRTUAL(UMapModifierData::Execute,);

};

UCLASS()
class TERRAINGEN_API UMapModifierDataNoise : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float parameterNoise = 1.f;

	//UFUNCTION()
	virtual void Execute() override;
};

UCLASS()
class TERRAINGEN_API UMapModifierDataRandom : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float parameterRandom = 1.f;

	//UFUNCTION()
	virtual void Execute() override;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMapModifierData*> InitialHeightModifier;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<UBaseHeightMapModifier*> InitialHeightModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<UBaseHeightMapModifier*> HeightPostProcessingModifier;*/


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float BiomeSeedPointDensity = 0.1f;

	float GetTotalWeighting() {
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
