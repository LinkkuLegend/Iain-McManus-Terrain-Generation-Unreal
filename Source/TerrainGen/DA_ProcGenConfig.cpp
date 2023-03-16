// Fill out your copyright notice in the Description page of Project Settings.


#include "DA_ProcGenConfig.h"

void UMapModifierDataNoise::Execute() {
	UE_LOG(LogTemp, Warning, TEXT("We are at noise."));
}

void UMapModifierDataRandom::Execute() {
	UE_LOG(LogTemp, Warning, TEXT("We are at random."));
}

