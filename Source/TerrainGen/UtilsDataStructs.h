// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



template <class T>
class MArray {

private:
	TArray<T> MultArray;
	short Columns, Rows;

public:
	MArray() {};
	MArray(T fillElement) : Columns(2), Rows(2) {
		MultArray.Init(fillElement, 4);
	};
	MArray(short columns, short rows) : Columns(columns), Rows(rows) {
		MultArray.Reserve(columns * rows);
	};
	MArray(T fillElement, short columns, short rows) : Columns(columns), Rows(rows) {
		int index = columns * rows;
		if(index <= 0)
			MultArray.Init(fillElement, 1);
		else {
			MultArray.Init(fillElement, columns * rows);
		}
	};
	MArray(TArray<T>& tarray, short columns, short rows) {
		if(tarray.IsEmpty()) {
			Columns = 0;
			Rows = 0;
			return;
		}

		if(columns * rows <= 0) {
			UE_LOG(LogTemp, Warning, TEXT("Columns or Rows numbers are invalid."));
			return;
		}

		if(columns * rows > tarray.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("In UtilsDataStructs.h, on the Constructor, we got an unstable array, so creation stopped."));
			return;
		}


		if(columns * rows < tarray.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("In UtilsDataStructs.h, on the Constructor, there are less elements than expected, is that on purpose?"));
		}


		/*MultArray.Init(tarray[0], tarray.Num());
		for(size_t i = 0; i < tarray.Num(); i++) {
			MultArray[i] = tarray[i];
		}*/
		MultArray.Empty();
		MultArray.Append(tarray);

		Columns = columns;
		Rows = rows;
	}
	~MArray() {
		MultArray.Empty();
	}

	T getItem(int x, int y) {
		int index = getIndex(x, y);
		if(MultArray.IsValidIndex(index)) {
			return MultArray[index];
		} else {
			UE_LOG(LogTemp, Warning, TEXT("In UtilsDataStructs.h, on getItem, we got out of range, that may be really bad."));
			return NULL;
		}
	}

	bool setItem(T item, int x, int y) {
		if(MultArray.IsEmpty())
			return false;

		if(x > Columns)
			return false;
		if(y > Rows)
			return false;

		int index = getIndex(x, y);
		if(MultArray.IsValidIndex(index)) {
			MultArray[index] = item;
			return true;
		}
		return false;
	}

	void PrintContent() {
		if(MultArray.IsEmpty())
			return;

		FString JoinedStr;
		TArray< FStringFormatArg > args;
		for(size_t y = 0; y < Rows; y++) {
			for(size_t x = 0; x < Columns; x++) {
				JoinedStr += FString::FromInt(getItem(x, y));
				JoinedStr += ' ';
			}
			UE_LOG(LogTemp, Warning, TEXT("%s "), *JoinedStr);
			JoinedStr.Empty();
			args.Empty();
		}
	}

	void PrintInfo() {
		if(!MultArray.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("Max Size: %d, Num of elements: %d. Dimensions: (%dx%d)"), MultArray.Max(), MultArray.Num(), Columns, Rows);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("No content on the array."));
		}
	}

private:

	int getIndex(int x, int y) {
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