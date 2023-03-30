// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilsDataStructs.h"
/*
UtilsDataStructs::UtilsDataStructs()
{
}

UtilsDataStructs::~UtilsDataStructs()
{
}
*/

template <>
void MArray<FString>::PrintContent() const {
	if(MultArray.IsEmpty())
		return;

	FString JoinedStr;
	for(size_t y = 0; y < Rows; y++) {
		for(size_t x = 0; x < Columns; x++) {
			JoinedStr += getItem(x, y);
			JoinedStr += ' ';
		}
		UE_LOG(LogTemp, Warning, TEXT("%s "), *JoinedStr);
		JoinedStr.Empty();
	}
}

template <>
void MArray<float>::PrintContent() const {
	if(MultArray.IsEmpty())
		return;
    
	FString JoinedStr;
	for(size_t y = 0; y < Rows; y++) {
		for(size_t x = 0; x < Columns; x++) {
			JoinedStr += "[" + FString::Printf(TEXT("%.1f"), getItem(x, y)) + "]";
		}
		UE_LOG(LogTemp, Warning, TEXT("%s "), *JoinedStr);
		JoinedStr.Empty();
	}

}



