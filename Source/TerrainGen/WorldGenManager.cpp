// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenManager.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/Engine/Classes/Components/SphereComponent.h>

//#if !UE_BUILD_DEBUG && !UE_BUILD_DEVELOPMENT
//#define ENABLE_RUNTIME_CHECKS 1
//#else
//#define ENABLE_RUNTIME_CHECKS 0
//#endif

// Sets default values
AWorldGenManager::AWorldGenManager() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	//WorldTerrainGen::PerlinNoiseGenCurves = Curves;

}

// Called when the game starts or when spawned
void AWorldGenManager::BeginPlay() {
	Super::BeginPlay();

	//Initialize WorldTerrainGen
	FTerrainGenCurves Curves;
	Curves.ContinentalnessCurve = ContinentalnessCurve;
	Curves.ErosionCurve = ErosionCurve;
	WorldTerrainGen::Initialize(Curves, Seed);


	// Get a pointer to the current player character
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// Get the current time before the function call
	const FDateTime StartTime = FDateTime::UtcNow();

	TargetTerrain->UpdateTerrain(PlayerCharacter->GetActorLocation());

	// Get the current time after the function call
	const FDateTime EndTime = FDateTime::UtcNow();
	// Calculate the execution time as a time span
	const FTimespan ExecutionTime = EndTime - StartTime;
	// Print the execution time in seconds
	UE_LOG(LogTemp, Log, TEXT("Total time: %f seconds"), ExecutionTime.GetTotalSeconds());


	//PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
	check(PlayerCharacter != nullptr);

	USphereComponent* SphereComponent = PlayerCharacter->FindComponentByClass<USphereComponent>();
	if(SphereComponent) {
		SphereComponent->SetSphereRadius(TargetTerrain->GetVisionRadius());
	}

	LastTimeExecuted = GetWorld()->GetTimeSeconds(); // Initialize the last time the code was executed to the current time
}

void AWorldGenManager::GenerateHeightMapByClusterEditor() {
	UE_LOG(LogTemp, Warning, TEXT("Generating Height Map at debug cluster: %dx%d"), DebugStartCluster.X, DebugStartCluster.Y);

	FDateTime StartTime = FDateTime::Now();

	FTerrainGenCurves Curves;
	Curves.ContinentalnessCurve = ContinentalnessCurve;
	Curves.ErosionCurve = ErosionCurve;
	WorldTerrainGen::Initialize(Curves, Seed);

	

	MArray<float> HeightMapContinentalness = WorldTerrainGen::GenerateClusterHeightMap(DebugStartCluster, DebugEndCluster, (uint8)MapType::Continentalness, BaseFrequencyContinentalness, ContinentalnessCurve, OctavesContinentalness);
	MArray<float> HeightMapErosion = WorldTerrainGen::GenerateClusterHeightMap(DebugStartCluster, DebugEndCluster, (uint8)MapType::Erosion, BaseFrequencyErosion, ErosionCurve, OctavesErosion);
	MArray<float> HeightMapPeeksAndValleys = WorldTerrainGen::GenerateClusterHeightMap(DebugStartCluster, DebugEndCluster, (uint8)MapType::Erosion, BaseFrequencyErosion, ErosionCurve, OctavesErosion);


	Continentalness = WorldTerrainGen::GenerateClusterTexture(HeightMapContinentalness, ContinentalnessCurve);
	Erosion = WorldTerrainGen::GenerateClusterTexture(HeightMapErosion, ErosionCurve);

	WorldHeight = WorldTerrainGen::MixMainTextures(HeightMapContinentalness, HeightMapErosion, ContinentalnessCurve);

	FDateTime EndTime = FDateTime::Now();
	float Duration = FPlatformTime::ToMilliseconds((EndTime - StartTime).GetTotalMilliseconds());
	UE_LOG(LogTemp, Warning, TEXT("MyFunction took %f ms"), Duration);

}

// Called every frame
void AWorldGenManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	double SpeedMagnitude = PlayerCharacter->GetVelocity().Length();
	// && !isUpdatingTerrain


	if(SpeedMagnitude > 0) { // OPTIMIZE Change the 1.0f depending of player velocity?
		if(GetWorld()->GetTimeSeconds() - LastTimeExecuted >= 1.0f) // Check if one second has elapsed since the last time the code was executed
		{
			const FVector PlayerPosition = PlayerCharacter->GetActorLocation();
			double start = GetWorld()->GetTimeSeconds();
			TargetTerrain->UpdateTerrain(PlayerPosition);
			//FGraphEventRef MyAsyncTask = FFunctionGraphTask::CreateAndDispatchWhenReady([PlayerPosition, this]() {
				//CallbackUpdateTerrain();
			//}, TStatId(), nullptr, ENamedThreads::AnyBackgroundHiPriTask);
			UE_LOG(LogTemp, Warning, TEXT("Time elapsed: %lf"), GetWorld()->GetTimeSeconds() - start);
			LastTimeExecuted = GetWorld()->GetTimeSeconds(); // Update the last time the code was executed to the current time
		}
	}



}

