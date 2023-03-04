// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FUtilsArray {

public:
    template< class T>
    static void ShuffleArray(TArray<T>& myArray) {
        int32 LastIndex = myArray.Num() - 1;
        for(int32 i = 0; i <= LastIndex; ++i) {
            int32 Index = FMath::RandRange(0, LastIndex);
            myArray.Swap(i, Index);
        }
    }

};








/**
class TERRAINGEN_API UtilsArray
{
public:
	UtilsArray();
	~UtilsArray();
};*/
