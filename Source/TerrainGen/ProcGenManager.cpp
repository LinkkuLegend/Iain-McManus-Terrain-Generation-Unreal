// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcGenManager.h"
#include "DA_BiomeConfig.h"
#include "Engine/Texture2D.h"

#include <TerrainGen/UtilsArray.h>



// Sets default values
AProcGenManager::AProcGenManager() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//CreateDefaultSubobject<UDynamicMesh>(TEXT("Terrain"));
	TargetTerrain = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("Terrain"));
	InitializeNeighbourOffsets();
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

bool AProcGenManager::IsTextureSafeToReadFrom(UTexture* Texture) {
	if(!Texture || !Texture->GetResource() || !Texture->GetResource()->TextureRHI) {
		return false;
	}
	return true;
};


void AProcGenManager::RegenerateTerrain() {
	UE_LOG(LogTemp, Warning, TEXT("Regenerating world..."));

	// Cache map resolution
	short HeightmapResolution{ 65 };

	PerformBiomeGeneration(HeightmapResolution);
}

void AProcGenManager::PerformBiomeGeneration(short HeightmapResolution) {
	// Reserving space in our Biome map and Strength map
	BiomeMap = MArray<short>(0, HeightmapResolution, HeightmapResolution);

	BiomeStrengths = MArray<float>(0.0f, HeightmapResolution, HeightmapResolution);

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

	UE_LOG(LogTemp, Warning, TEXT("Creating Texture."));

	// Setup a Texture2D to display the Biomes
	BiomeMapTexture = UTexture2D::CreateTransient(HeightmapResolution, HeightmapResolution); //PF_R8G8B8A8
	FColor* FormatedImageData = static_cast<FColor*>(BiomeMapTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));


	//BiomeMap.PrintInfo();
	//BiomeMap.PrintContent();

	for(size_t y = 0; y < HeightmapResolution; y++) {
		for(size_t x = 0; x < HeightmapResolution; x++) {
			float BiomeHue = (BiomeMap.getItem(x, y) ) / ((Config->GetNumBiomes()) * 1.0f);
			FLinearColor BaseColor(BiomeHue * 360.f,0.75f,0.75f);
			FormatedImageData[y * HeightmapResolution + x] = BaseColor.HSVToLinearRGB().ToFColor(false);
		}
	}



	BiomeMapTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	BiomeMapTexture->UpdateResource();

	while(!IsTextureSafeToReadFrom(BiomeMapTexture)) {}

	BiomeImageToDisk(); //In Blueprint

	UE_LOG(LogTemp, Warning, TEXT("Finished."));
}

/*
* Uses Ooze based generation from here: https://www.procjam.com/tutorials/en/ooze/
*/

void AProcGenManager::PerformSpawnIndividualBiome(short BiomeID, short HeightmapResolution) {
	//UE_LOG(LogTemp, Warning, TEXT("Got a Biome ID: %d"), BiomeID);
	// Pick a random spawn location, here we can add spawn restrictions in the future
	FInt32Vector2 spawnLocation(
		FMath::RandRange(0, HeightmapResolution-1),
		FMath::RandRange(0, HeightmapResolution-1)
	);

	// Cache biome configuration
	UDA_BiomeConfig* BiomeConfig = Config->Biomes[BiomeID].Biome;

	// Pick the starter intensity
	float startIntensity = FMath::RandRange(BiomeConfig->MinIntensity, BiomeConfig->MaxIntensity);

	//if(BiomeID != 1)
	//	return;

	// Setup working list
	TQueue<FInt32Vector2> WorkingList;
	WorkingList.Enqueue(spawnLocation);

	// Setup Visited vector for the Oozing algorith, if we have visited one node already, we don't want to ooze it again, right? ;)
	TSet<FInt32Vector2> VisitedMapSet;
	


	// Setup intensity map, for controlling the spread
	//MArray<float> TargetIntensity(0.0f, HeightmapResolution, HeightmapResolution);
	//TargetIntensity.setItem(startIntensity, spawnLocation.X, spawnLocation.Y); // Setup the starting intensity
	TMap<FInt32Vector2,float> TargetIntensityMap;
	TargetIntensityMap.Add(spawnLocation, startIntensity);

	//int NumVisited = 0;
	// Let the Oozing begin
	while(!WorkingList.IsEmpty()) {
		FInt32Vector2 workingLocation;
		WorkingList.Dequeue(workingLocation);


		
		// Set the biome
		BiomeMap.setItem(BiomeID, workingLocation.X, workingLocation.Y);
		//VisitedMap.setItem(true, workingLocation.X, workingLocation.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Setting Biome ID: %d at %dx%d"), BiomeID, workingLocation.X, workingLocation.Y);
		/*BiomeStrengths.setItem(
			TargetIntensity.getItem(workingLocation.X, workingLocation.Y),
			workingLocation.X,
			workingLocation.Y
		);*/

		
		// Traverse the neighbours
		for(int neighbourIndex = 0; neighbourIndex < NeighbourOffsets.Num(); ++neighbourIndex) {

			FInt32Vector2 NeightbourLocation(workingLocation.X + NeighbourOffsets[neighbourIndex].X, workingLocation.Y + NeighbourOffsets[neighbourIndex].Y);

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
			float decayAmount = decayRate *  magnitude;

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


// Called when the game starts or when spawned
void AProcGenManager::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AProcGenManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

