// Fill out your copyright notice in the Description page of Project Settings.


#include "DA_BiomeConfig.h"

UBiomeTexture::UBiomeTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	// Initialize any member variables here
	UniqueID = TEXT("");
	Albedo = nullptr;
	NormalMap = nullptr;
}