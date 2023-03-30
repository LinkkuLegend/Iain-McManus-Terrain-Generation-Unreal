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
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		EMapModifierType ModifierType = EMapModifierType::Noise;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", AllowPrivateAccess = "true"))
		float Strength = 1.f;

public:
	//UFUNCTION()
	//virtual void Execute(int MapResolution, MArray<float> MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) PURE_VIRTUAL(UMapModifierData::Execute, );
	virtual void Execute(int MapResolution, MArray<float> &MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) PURE_VIRTUAL(UMapModifierData::Execute, );

};

UCLASS()
class TERRAINGEN_API UMapModifierDataNoise : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float NoiseAmount = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float> &MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierDataRandom : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float HeightDelta = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float> &MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierOffSet : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float OffsetAmount = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float> &MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};

UCLASS()
class TERRAINGEN_API UMapModifierSetValue : public UMapModifierData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float TargetHeight = 0.f;

	//UFUNCTION()
	virtual void Execute(int MapResolution, MArray<float> &MapHeights, FVector MapScale, MArray<uint8> BiomeMap = MArray<uint8>(), int BiomeIndex = -1, UDA_BiomeConfig* BiomeConfig = nullptr) override;
};