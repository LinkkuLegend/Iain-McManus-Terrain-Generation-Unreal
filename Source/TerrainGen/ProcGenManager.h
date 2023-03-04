// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DA_ProcGenConfig.h"
#include "Components/DynamicMeshComponent.h"
#include "UtilsDataStructs.h"
#include "ProcGenManager.generated.h"


UCLASS()
class TERRAINGEN_API AProcGenManager : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProcGenManager();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GenConfig", meta = (AllowPrivateAccess = "true"))
		UDA_ProcGenConfig* Config;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain", meta = (AllowPrivateAccess = "true"))
		class UDynamicMeshComponent* TargetTerrain;

	UFUNCTION(BlueprintCallable)
		void RegenerateTerrain();

	void PerformBiomeGeneration(short mapResolution);

	void PerformSpawnIndividualBiome(short BiomeIndex, short HeightmapResolution);

	void InitializeNeighbourOffsets();

	bool IsTextureSafeToReadFrom(UTexture* Texture);

#pragma region "Mapgen Variables"

	MArray <short> BiomeMap;
	MArray <float> BiomeStrengths;

	TArray<FInt32Vector2> NeighbourOffsets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		UTexture2D* BiomeMapTexture;

#pragma endregion

public:
	UFUNCTION(BlueprintImplementableEvent)
		void BiomeImageToDisk();


	


};
