// Fill out your copyright notice in the Description page of Project Settings.




#include "ProcGenManager.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Terrain.h"


// Sets default values
AProcGenManager::AProcGenManager() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//RootComponent->Mobility = EComponentMobility::Static;
	//CreateDefaultSubobject<UDynamicMesh>(TEXT("Terrain"));
	TestTerrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	//RootComponent = TargetTerrain;
	//TargetTerrain->SetCollisionProfileName("BlockAll");
	SetRootComponent(TestTerrain);
	//TargetTerrain->bUseAsyncCooking = true;

	InitializeNeighbourOffsets();
}

// Called when the game starts or when spawned
void AProcGenManager::BeginPlay() {
	Super::BeginPlay();
	//GenerateTile();

}

// Called every frame
void AProcGenManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	//TargetTerrain->UpdateTerrain(GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
}

void AProcGenManager::InitializeNeighbourOffsets() {
	NeighbourOffsets.Add(FInt32Vector2(1, 0));		//E
	NeighbourOffsets.Add(FInt32Vector2(0, -1));		//N
	NeighbourOffsets.Add(FInt32Vector2(-1, 0));		//W
	NeighbourOffsets.Add(FInt32Vector2(0, 1));		//S
	NeighbourOffsets.Add(FInt32Vector2(1, -1));		//NE
	NeighbourOffsets.Add(FInt32Vector2(-1, -1));	//NW
	NeighbourOffsets.Add(FInt32Vector2(-1, 1));		//SW
	NeighbourOffsets.Add(FInt32Vector2(1, 1));		//SE
}

bool AProcGenManager::IsTextureSafeToReadFrom(UTexture2D* Texture) {
	if(!Texture || !Texture->GetResource() || !Texture->GetResource()->TextureRHI) {
		return false;
	}
	return true;
};

void AProcGenManager::RegenerateTerrain() {
	UE_LOG(LogTemp, Warning, TEXT("Regenerating world..."));

	uint32 targetResolution{ 257 };

	// Generate the low resolution biome map
	PerformBiomeGeneration_LowRes(Config->GetBiomeMapResolution());

	// Generate the high resolution biome map
	PerformBiomeGeneration_HighRes(Config->GetBiomeMapResolution(), targetResolution);

	// Update the terrain height
	//Perform_HeightMapModification(targetResolution);
}

void AProcGenManager::PerformBiomeGeneration_LowRes(uint16 HeightmapResolution) {
	// Reserving space in our Biome map and Strength map
	BiomeMap_LowRes = MArray<uint8>(0, HeightmapResolution, HeightmapResolution);
	BiomeStrengths_LowRes = MArray<float>(0.0f, HeightmapResolution, HeightmapResolution);

	// Setup space for the seed points
	TArray<short> biomesToSpawn;
	int numSeedPoints = FMath::FloorToInt(HeightmapResolution * HeightmapResolution * Config->BiomeSeedPointDensity);
	biomesToSpawn.Reserve(numSeedPoints);

	// Populate the biomes to spawn based on weightings
	float TotalBiomeWeighting = Config->GetTotalWeighting();
	for(int biomeIndex = 0; biomeIndex < Config->GetNumBiomes(); ++biomeIndex) {
		int numEntries = FMath::RoundToInt(numSeedPoints * Config->Biomes[biomeIndex].Weighting / TotalBiomeWeighting);
		UE_LOG(LogTemp, Warning, TEXT("Will spawn: %d seedpoints for %s"), numEntries, *Config->Biomes[biomeIndex].Biome->name);

		for(int entryIndex = 0; entryIndex < numEntries; ++entryIndex) {
			biomesToSpawn.Add(biomeIndex);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Phase one completed."));

	// Shuffling the biomes
	FUtilsArray::ShuffleArray(biomesToSpawn);

	// Spawn the individual biomes
	while(biomesToSpawn.Num() > 0) {
		//for(size_t i = 0; i < 10; ++i) {



			// Extract the biome index
		int biomeArrayIndex = biomesToSpawn.Num() - 1;
		short biomeID = biomesToSpawn[biomeArrayIndex];
		biomesToSpawn.RemoveAt(biomeArrayIndex, 1, false);

		// Remove seed point
		//UE_LOG(LogTemp, Warning, TEXT("Sending a Biome ID: %d"), biomeID);
		PerformSpawnIndividualBiome(biomeID, HeightmapResolution);
	}

	/*
	UE_LOG(LogTemp, Warning, TEXT("Creating Texture."));

	// Setup a Texture2D to display the Biomes for debug purposes
	BiomeMapTextureLowRes = UTexture2D::CreateTransient(HeightmapResolution, HeightmapResolution); //PF_R8G8B8A8
	FColor* FormatedImageData = static_cast<FColor*>(BiomeMapTextureLowRes->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));


	//BiomeMap.PrintInfo();
	//BiomeMap.PrintContent();

	for(size_t y = 0; y < HeightmapResolution; y++) {
		for(size_t x = 0; x < HeightmapResolution; x++) {
			float BiomeHue = (BiomeMap_LowRes.getItem(x, y) ) / ((Config->GetNumBiomes()) * 1.0f);
			FLinearColor BaseColor(BiomeHue * 360.f,0.75f,0.75f);
			FormatedImageData[y * HeightmapResolution + x] = BaseColor.HSVToLinearRGB().ToFColor(false);
		}
	}



	BiomeMapTextureLowRes->GetPlatformData()->Mips[0].BulkData.Unlock();
	BiomeMapTextureLowRes->UpdateResource();

	while(!IsTextureSafeToReadFrom(BiomeMapTextureLowRes)) {}*/


	DebugBiomeToTexture(BiomeMapTextureLowRes, BiomeMap_LowRes, Config->GetBiomeMapResolution(), Config->GetNumBiomes(), "BiomeMap_LowRes");



	UE_LOG(LogTemp, Warning, TEXT("Finished."));
}

void AProcGenManager::DebugBiomeToTexture(UTexture2D*& texture, MArray<uint8> pixels, uint16 resolution, uint8 NumBiomes, FString filename) {

	UE_LOG(LogTemp, Warning, TEXT("Creating Texture."));

	// Setup a Texture2D to display the Biomes for debug purposes
	texture = UTexture2D::CreateTransient(resolution, resolution); //PF_R8G8B8A8
	FColor* FormatedImageData = static_cast<FColor*>(texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));


	//pixels.PrintInfo();
	//pixels.PrintContent();

	for(size_t y = 0; y < resolution; y++) {
		for(size_t x = 0; x < resolution; x++) {
			float BiomeHue = (pixels.getItem(x, y)) / ((NumBiomes) * 1.0f);
			FLinearColor BaseColor(BiomeHue * 360.f, 0.75f, 0.75f);
			FormatedImageData[y * resolution + x] = BaseColor.HSVToLinearRGB().ToFColor(false);
		}
	}



	texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	texture->UpdateResource();

	while(!IsTextureSafeToReadFrom(texture)) {}

	BiomeImageToDisk(texture, filename); //In Blueprint
}

/*
* Uses Ooze based generation from here: https://www.procjam.com/tutorials/en/ooze/
*/
void AProcGenManager::PerformSpawnIndividualBiome(short BiomeID, uint16 HeightmapResolution) {
	//UE_LOG(LogTemp, Warning, TEXT("Got a Biome ID: %d"), BiomeID);
	// Pick a random spawn location, here we can add spawn restrictions in the future
	FUint32Vector2 spawnLocation(
		FMath::RandRange(0, HeightmapResolution - 1),
		FMath::RandRange(0, HeightmapResolution - 1)
	);

	// Cache biome configuration
	UDA_BiomeConfig* BiomeConfig = Config->Biomes[BiomeID].Biome;

	// Pick the starter intensity
	float startIntensity = FMath::RandRange(BiomeConfig->MinIntensity, BiomeConfig->MaxIntensity);

	//if(BiomeID != 1)
	//	return;

	// Setup working list
	TQueue<FUint32Vector2> WorkingList;
	WorkingList.Enqueue(spawnLocation);

	// Setup Visited vector for the Oozing algorith, if we have visited one node already, we don't want to ooze it again, right? ;)
	TSet<FUint32Vector2> VisitedMapSet;



	// Setup intensity map, for controlling the spread
	//MArray<float> TargetIntensity(0.0f, HeightmapResolution, HeightmapResolution);
	//TargetIntensity.setItem(startIntensity, spawnLocation.X, spawnLocation.Y); // Setup the starting intensity
	TMap<FUint32Vector2, float> TargetIntensityMap;
	TargetIntensityMap.Add(spawnLocation, startIntensity);

	//int NumVisited = 0;
	// Let the Oozing begin
	while(!WorkingList.IsEmpty()) {
		FUint32Vector2 workingLocation;
		WorkingList.Dequeue(workingLocation);



		// Set the biome
		BiomeMap_LowRes.setItem(BiomeID, workingLocation.X, workingLocation.Y);
		//VisitedMap.setItem(true, workingLocation.X, workingLocation.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Setting Biome ID: %d at %dx%d"), BiomeID, workingLocation.X, workingLocation.Y);
		/*BiomeStrengths.setItem(
			TargetIntensity.getItem(workingLocation.X, workingLocation.Y),
			workingLocation.X,
			workingLocation.Y
		);*/


		// Traverse the neighbours
		for(int neighbourIndex = 0; neighbourIndex < NeighbourOffsets.Num(); ++neighbourIndex) {

			FUint32Vector2 NeightbourLocation(workingLocation.X + NeighbourOffsets[neighbourIndex].X, workingLocation.Y + NeighbourOffsets[neighbourIndex].Y);


			// Skip if invalid
			if(NeightbourLocation.X < 0 || NeightbourLocation.Y < 0 || NeightbourLocation.X >= HeightmapResolution || NeightbourLocation.Y >= HeightmapResolution)
				continue;

			//Skip if visited
			if(VisitedMapSet.Contains(NeightbourLocation))
				continue;

			VisitedMapSet.Add(NeightbourLocation);

			// Work out and store neighbour strength
			float magnitude = 1.f; // The first 4 in NeighbourOffsets are the cardinal movements, so the cost is lower. Also this is for making the Ooze round-like.
			if(neighbourIndex >= 4)
				magnitude = 1.4142f;
			float decayRate = FMath::RandRange(BiomeConfig->MinDecayRate, BiomeConfig->MaxDecayRate);
			float decayAmount = decayRate * magnitude;

			//float NeightbourStrength = TargetIntensity.getItem(workingLocation.X, workingLocation.Y) - decayAmount;
			//TargetIntensity.setItem(NeightbourStrength, NeightbourLocation.X, NeightbourLocation.Y);

			float NeightbourStrength = TargetIntensityMap[workingLocation] - decayAmount;
			TargetIntensityMap.Add(NeightbourLocation, NeightbourStrength);

			if(NeightbourStrength <= 0.0f) { // If strength is too low, the ooze doesn't reach
				continue;
			}

			WorkingList.Enqueue(NeightbourLocation);
		}


	}
	//UE_LOG(LogTemp, Warning, TEXT("Visited a total of %d times."), NumVisited);

}

uint8 AProcGenManager::CalculateHighResBiomeIndex(uint16 lowResMapSize, uint32 lowResX, uint32 lowResY, uint32 fractionX, uint32 fractionY) {
	float A = BiomeMap_LowRes.getItem(lowResX, lowResY);
	float B = (lowResX + 1) < lowResMapSize ? BiomeMap_LowRes.getItem(lowResX + 1, lowResY) : A;
	float C = (lowResY + 1) < lowResMapSize ? BiomeMap_LowRes.getItem(lowResX, lowResY + 1) : A;
	float D = 0;

	if((lowResX + 1) >= lowResMapSize) {
		D = C;
	} else if((lowResY + 1) >= lowResMapSize) {
		D = B;
	} else {
		D = BiomeMap_LowRes.getItem(lowResX + 1, lowResY + 1);
	}


	// Perform bilinear filter
	float index = A * (1 - fractionX) * (1 - fractionY) + B * fractionX * (1 - fractionY) +
		C * (1 - fractionX) * fractionY + D * fractionX * fractionY;

	// Build an array of the possible biomes baed on the values used to interpolate
	TArray<float> candidateBiomes = { A, B, C, D };
	// Find the neighbouring biome closesttothe interpolated biome
	float bestBiome = -1.f;
	float bestDelta = TNumericLimits<float>::Max();


	for(uint8 biomeIndex = 0; biomeIndex < candidateBiomes.Num(); biomeIndex++) {
		float delta = FMath::Abs(index - candidateBiomes[biomeIndex]);

		if(delta < bestDelta) {
			bestDelta = delta;
			bestBiome = candidateBiomes[biomeIndex];
		}
	}

	//return BiomeMap_LowRes.getItem(lowResX, lowResY);
	return FMath::RoundToInt(bestBiome);
}

void AProcGenManager::PerformBiomeGeneration_HighRes(uint16 lowResMapResolution, uint32 targetResolution) {
	// Reserving space in our Biome map and Strength map
	BiomeMap = MArray<uint8>(0, targetResolution, targetResolution);
	BiomeStrengths = MArray<float>(0.0f, targetResolution, targetResolution);

	// Calculate map scale
	float mapScale = lowResMapResolution / (targetResolution * 1.0f);
	//uint32 imageSize = FMath::FloorToInt(targetResolution * mapScale);
	UE_LOG(LogTemp, Warning, TEXT("Map Scale: %f"), mapScale);

	// Calculate the high res map
	for(uint32 y = 0; y < targetResolution; ++y) {
		uint32 lowResY = FMath::FloorToInt(y * mapScale);
		float fractionY = y * mapScale - lowResY;
		for(uint32 x = 0; x < targetResolution; ++x) {
			uint32 lowResX = FMath::FloorToInt(x * mapScale);
			float fractionX = x * mapScale - lowResX;


			BiomeMap.setItem(CalculateHighResBiomeIndex(lowResMapResolution, lowResX, lowResY, fractionX, fractionY), x, y);

			// No interpolation -- Point Based
			//BiomeMap.setItem(BiomeMap_LowRes.getItem(lowResX, lowResY), x, y);
		}
	}

	/*for(uint32 y = targetResolution - imageSize; y < targetResolution; ++y) {
		uint32 lowResY = FMath::FloorToInt(y * mapScale);
		for(uint32 x = 0; x < targetResolution; ++x) {
			uint32 lowResX = FMath::FloorToInt(x * mapScale);

			BiomeMap.setItem(BiomeMap_LowRes.getItem(lowResX, lowResY), x, y);

			// No interpolation -- Point Based
			//BiomeMap.setItem(BiomeMap_LowRes.getItem(lowResX, lowResY), x, y);
		}
	}

	for(uint32 y = 0; y < targetResolution; ++y) {
		uint32 lowResY = FMath::FloorToInt(y * mapScale);
		for(uint32 x = targetResolution - imageSize; x < targetResolution; ++x) {
			uint32 lowResX = FMath::FloorToInt(x * mapScale);

			BiomeMap.setItem(BiomeMap_LowRes.getItem(lowResX, lowResY), x, y);

			// No interpolation -- Point Based
			//BiomeMap.setItem(BiomeMap_LowRes.getItem(lowResX, lowResY), x, y);
		}
	}*/



	DebugBiomeToTexture(BiomeMapTextureHighRes, BiomeMap, targetResolution, Config->GetNumBiomes(), "BiomeMap_HighRes");

}

void AProcGenManager::Perform_HeightMapModification(int32 targetResolution) {

	MArray<float> HeightMap;

	TargetTerrain->GetHeights(0,0,3,3, HeightMap);

	//Execute any initial height modifiers
	if(Config != NULL && !Config->InitialHeightModifier.IsEmpty()) {
		for(int8 x = 0; x < Config->InitialHeightModifier.Num(); x++) {
			Config->InitialHeightModifier[x]->Execute(targetResolution, HeightMap, FVector());
		}
	}

	//Execute height modifiers per biome
	for(int BiomeIndex = 0; BiomeIndex < Config->Biomes.Num(); BiomeIndex++) {
		UDA_BiomeConfig* Biome = Config->Biomes[BiomeIndex].Biome;
		for(int8 x = 0; x < Biome->BiomeHeightModifier.Num(); x++) {
			Biome->BiomeHeightModifier[x]->Execute(targetResolution, HeightMap, FVector(), BiomeMap, BiomeIndex, Biome);
		}
	}


	//Execute any post processing height modifiers
	if(Config != NULL && !Config->PostProcessingHeightModifier.IsEmpty()) {
		for(int8 x = 0; x < Config->PostProcessingHeightModifier.Num(); x++) {
			Config->PostProcessingHeightModifier[x]->Execute(targetResolution, HeightMap, FVector());
		}
	}

}

/*
* Code from: https://www.bluefruitgames.com/creating-a-procedural-mesh-in-unreal-engine/
*/

void AProcGenManager::GenerateTile() {

	UE_LOG(LogTemp, Warning, TEXT("Start of GenerateTile()"));

	/*TArray<FVector> vertices;
	vertices.Add(FVector(0, 0, 0));
	vertices.Add(FVector(0, 100, 0));
	vertices.Add(FVector(0, 0, 100));

	TArray<int32> triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	TArray<FVector2D> UV0;
	UV0.Add(FVector2D(0, 0));
	UV0.Add(FVector2D(10, 0));
	UV0.Add(FVector2D(0, 10));


	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));

	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

	UE_LOG(LogTemp, Warning, TEXT("Testing"));
	UE_LOG(LogTemp, Warning, TEXT("Sections: %d"), TargetTerrain->GetNumSections());*/

	//TargetTerrain->CreateMeshSection_LinearColor
	TestTerrain->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);

	//TargetTerrain->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);


	// Enable collision data
	//TargetTerrain->ContainsPhysicsTriMeshData(true);

}





