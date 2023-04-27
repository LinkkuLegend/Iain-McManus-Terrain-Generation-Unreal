// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



template <class T>
class MArray {

private:
	TArray<T> MultArray;
	uint32 Columns, Rows;

public:
	MArray() {};
	MArray(T fillElement) : Columns(2), Rows(2) {
		MultArray.Init(fillElement, 4);
	};
	MArray(uint32 columns, uint32 rows) : Columns(columns), Rows(rows) {
		MultArray.Init({}, columns * rows);
	};
	MArray(T fillElement, uint32 columns, uint32 rows) : Columns(columns), Rows(rows) {
		int index = columns * rows;
		if(index <= 0)
			MultArray.Init(fillElement, 1);
		else {
			MultArray.Init(fillElement, columns * rows);
		}
	};
	MArray(TArray<T>& tarray, uint32 columns, uint32 rows) {
		if(tarray.IsEmpty()) {
			Columns = 0;
			Rows = 0;
			return;
		}

		if(columns * rows <= 0) {
			UE_LOG(LogTemp, Error, TEXT("Columns or Rows numbers are invalid."));
			return;
		}

		if(columns * rows > tarray.Num()) {
			UE_LOG(LogTemp, Error, TEXT("In UtilsDataStructs.h, on the Constructor, we got an unstable array, so creation stopped."));
			return;
		}


		if(columns * rows < tarray.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("In UtilsDataStructs.h, on the Constructor, there are less elements than expected, is that on purpose?"));
		}

		MultArray.Empty();
		MultArray.Append(tarray);

		Columns = columns;
		Rows = rows;
	}
	~MArray() {
		MultArray.Empty();
	}

	T getItem(uint32 x, uint32 y) const {
		int index = getIndex(x, y);
		if(MultArray.IsValidIndex(index)) {
			return MultArray[index];
		} else {
			UE_LOG(LogTemp, Warning, TEXT("In UtilsDataStructs.h, on getItem, we got an invalid index, values were: %d/%d."), x, y);
			return {};
		}

	}

	bool setItem(T item, uint32 x, uint32 y) {

		if(x > Columns)
			return false;
		if(y > Rows)
			return false;

		int index = getIndex(x, y);
		if(MultArray.IsValidIndex(index)) {
			MultArray[index] = item;
			return true;
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Invalid Index at %dx%d"), x, y);
		}
		return false;
	}


	void PrintContent() const;

	void PrintInfo() {
		if(!MultArray.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("Max Size: %d, Num of elements: %d. Dimensions: (%dx%d)"), MultArray.Max(), MultArray.Num(), Columns, Rows);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("No content on the array."));
		}
	}

	void Append(MArray<T> AppendableArray, int offsetX, int offsetY) {
		if(AppendableArray.Rows + offsetY > Rows) {
			UE_LOG(LogTemp, Warning, TEXT("Append Array too many rows. Our rows: %d. Append Rows: %d. OffsetY: %d"),
				   this->Rows, AppendableArray.Rows, offsetY);
			return;
		}

		if(AppendableArray.Columns + offsetX > Columns) {
			UE_LOG(LogTemp, Warning, TEXT("Append Array too many columns."));
			return;
		}

		for(uint32 y = 0; y < AppendableArray.Rows; y++) {
			for(uint32 x = 0; x < AppendableArray.Columns; x++) {
				setItem(AppendableArray.getItem(x, y), x + offsetX, y + offsetY);
			}
		}
	}

	// x = Columns; y = Rows
	FUintVector2 GetArraySize() const {
		return FUintVector2(Columns, Rows);
	}

	MArray<T> getArea(int OffsetX, int OffsetY, uint8 SizeX, uint8 SizeY) const {
		FUintVector2 Sizes = GetArraySize();

		check(static_cast<uint32>(OffsetX) + SizeX < Sizes.X);
		check(static_cast<uint32>(OffsetY) + SizeY < Sizes.Y);

		MArray<T> AreaArray = MArray<T>(SizeX + 1, SizeY + 1);
		for(int y = OffsetY; y < OffsetY + SizeY + 1; y++) {
			for(int x = OffsetX; x < OffsetX + SizeX + 1; x++) {
				AreaArray.setItem(getItem(x,y), x - OffsetX, y - OffsetY);
			}
		}


		return AreaArray;
	}

	SIZE_T GetResourceSizeBytes() {
		SIZE_T SizeInBytes = (MultArray.Num() * sizeof(T)) + (sizeof(uint32) * 2);
		return SizeInBytes;
	}

private:

	int getIndex(int x, int y) const {
		return y * Columns + x;
	}

};

/**
 *

class TERRAINGEN_API UtilsDataStructs {
public:
	UtilsDataStructs();
	~UtilsDataStructs();
};
*/