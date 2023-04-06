// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "UtilsDataStructs.h"

#include "MapModifierData.generated.h"

class UDA_BiomeConfig;

/**
 *
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class TERRAINGEN_API UMapModifierData : public UObject {

	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", AllowPrivateAccess = "true"))
		float Strength = 1.f;

public:
	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) PURE_VIRTUAL(UMapModifierData::Execute, );

};

UCLASS()
class TERRAINGEN_API UMapModifierDataNoise : public UMapModifierData {

	GENERATED_BODY()

public:
	// Determines the overall magnitude of the noise
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float NoiseAmount = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BaseXFrequency = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BaseYFrequency = 1.f;

	// Determines the number of layers of noise to add together
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 octaves = 1; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float XFrequencyVariationPerOctane = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float YFrequencyVariationPerOctane = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float NoiseAmountVariationPerOctane = 1;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//	float persistence = 1.f; // Determines how quickly the amplitude of the noise decreases with each additional layer

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierDataRandom : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float HeightDelta = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierOffSet : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float OffsetAmount = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierSetValue : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float TargetHeight = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierSmooth : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int SmoothKernelSize = 5;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float>& MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};