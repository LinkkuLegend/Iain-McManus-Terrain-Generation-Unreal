// Copyright Epic Games, Inc. All Rights Reserved.

#include "TerrainGenGameMode.h"
#include "TerrainGenCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATerrainGenGameMode::ATerrainGenGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/_Game/Mapgen/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
