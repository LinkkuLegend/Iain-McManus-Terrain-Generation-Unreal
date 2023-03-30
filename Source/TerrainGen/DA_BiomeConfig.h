// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapModifierData.h"
#include "DA_BiomeConfig.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class TERRAINGEN_API UDA_BiomeConfig : public UDataAsset {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MinIntensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MaxIntensity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MinDecayRate = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MaxDecayRate = 0.02f;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
		//UDA_BaseHeightMapModifier* HeightModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMapModifierData*> BiomeHeightModifier;

};
