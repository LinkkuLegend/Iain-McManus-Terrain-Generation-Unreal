// Fill out your copyright notice in the Description page of Project Settings.

#include "TerrainAlgorithms.h"
#include "Containers/Set.h"



MArray<float> TerrainAlgorithms::CalculateWaterAreas(const MArray <float>& HeightMap) {
	HeightMapVariables HMVariables;
	CalculateVariables(HeightMap, HMVariables);

	UE_LOG(LogTemp, Warning, TEXT("Min: %f Max: %f"), HMVariables.MinValue, HMVariables.MaxValue);
	UE_LOG(LogTemp, Warning, TEXT("Mean: %f"), HMVariables.Mean);
	UE_LOG(LogTemp, Warning, TEXT("Standard deviation: %lf"), HMVariables.StandardDeviation);

	//This uses the Mean and SD, which can be a bad heuristic for this case, if the Curve is changed drastically, this can lead to unwanted results
	float FloodStartThreshold = HMVariables.Mean - HMVariables.StandardDeviation;

	FUintVector2 ArraySize = HeightMap.GetArraySize();
	MArray<bool> CheckedAreas(false, ArraySize.X, ArraySize.Y);
	MArray<float> FloodMap(0, ArraySize.X, ArraySize.Y);
	MArray<float> FinalMap(0, ArraySize.X, ArraySize.Y);

	TArray<TArray<FIntPoint>> Shapes;

	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {
			if(CheckedAreas.getItem(x, y)) { //We check if this position was explored in the Flood function already, if it was, skipping
				continue;
			}

			if(HeightMap.getItem(x, y) < FloodStartThreshold) {
				UE_LOG(LogTemp, Warning, TEXT("Flooding at: %d x %d"), x, y);

				Flood(HeightMap, FloodMap, CheckedAreas, FIntPoint(x, y), FloodStartThreshold);
				/*FloodMap = MArray<float>(0, 3, 3);
				FloodMap.setItem(-1, 1, 1);*/

				Shapes.Add(GetBorderFromFloodMap(FloodMap));

				for(size_t sy = 0; sy < FloodMap.GetArraySize().Y; sy++) {
					for(size_t sx = 0; sx < FloodMap.GetArraySize().Y; sx++) {
						if(FloodMap.getItem(sx, sy) == -1) {
							FinalMap.setItem(-1, sx, sy);
						}
					}
				}

				FloodMap = MArray<float>(0, ArraySize.X, ArraySize.Y);
			}
		}
	}

	for(TArray<FIntPoint> ShapeArray : Shapes) {
		for(FIntPoint ShapePoint : ShapeArray) {
			FinalMap.setItem(1, ShapePoint.X, ShapePoint.Y);
		}
	}

	return FinalMap;
}

TArray<FIntPoint> TerrainAlgorithms::GetBorderFromFloodMap(const MArray<float>& FloodMap) {

	FUintVector2 ArraySize = FloodMap.GetArraySize();
	MArray<int8> ShapeMap(0, ArraySize.X, ArraySize.Y);

	//Make the border
	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {

			// Top cell
			if(y > 0 && FloodMap.getItem(x, y) != FloodMap.getItem(x, y - 1) && FloodMap.getItem(x, y) == 0) {
				ShapeMap.setItem(1, x, y);
			}
			// Bottom cell
			if(y < ArraySize.Y - 1 && FloodMap.getItem(x, y) != FloodMap.getItem(x, y + 1) && FloodMap.getItem(x, y) == 0) {
				ShapeMap.setItem(1, x, y);
			}
			// Left cell
			if(x > 0 && FloodMap.getItem(x, y) != FloodMap.getItem(x - 1, y) && FloodMap.getItem(x, y) == 0) {
				ShapeMap.setItem(1, x, y);
			}
			// Right cell
			if(x < ArraySize.X - 1 && FloodMap.getItem(x, y) != FloodMap.getItem(x + 1, y) && FloodMap.getItem(x, y) == 0) {
				ShapeMap.setItem(1, x, y);
			}

		}
	}

	//// Top and bottom borders
	//for(uint32 x = 0; x < ArraySize.X; x++) {
	//	if(FloodMap.getItem(x, 0) == -1)
	//		ShapeMap.setItem(1, x, 0);

	//	if(FloodMap.getItem(x, ArraySize.Y - 1) == -1)
	//		ShapeMap.setItem(1, x, ArraySize.Y - 1);
	//}

	//// Right and left Border
	//for(uint32 y = 0; y < ArraySize.Y; y++) {
	//	if(FloodMap.getItem(0, y) == -1)
	//		ShapeMap.setItem(1, 0, y);

	//	if(FloodMap.getItem(ArraySize.X - 1, y) == -1)
	//		ShapeMap.setItem(1, ArraySize.X - 1, y);
	//}

	/*UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), ShapeMap.getItem(0, 0), ShapeMap.getItem(1, 0), ShapeMap.getItem(2, 0));
	UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), ShapeMap.getItem(0, 1), ShapeMap.getItem(1, 1), ShapeMap.getItem(2, 1));
	UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), ShapeMap.getItem(0, 2), ShapeMap.getItem(1, 2), ShapeMap.getItem(2, 2));*/

	//Pick a first start point 
	//We could have done this in the previous loop, but this in cleaner, but... maybe less efficient?
	//We need a second point, that will determine the direction from where we would calculate the border
	FIntPoint StartingPoint(0, 0);
	FIntPoint SpreadPoint(0, 0);
	bool found = false;

	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {
			if(ShapeMap.getItem(x, y) == 1 && BorderPointsInOrder(ShapeMap, FIntPoint(x, y), { FIntPoint(x, y) }).Num() == 1) {
				StartingPoint = FIntPoint(x, y);
				SpreadPoint = BorderPointsInOrder(ShapeMap, FIntPoint(x, y), { FIntPoint(x, y) })[0];
				found = true;
				break;
			}
		}
		if(found)
			break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting Point: %d x %d, Spread Point: %d x %d"), StartingPoint.X, StartingPoint.Y, SpreadPoint.X, SpreadPoint.Y);

	//Get the neightbour, so it is order
	TArray<FIntPoint> RiverShapeVertex;
	TArray<FIntPoint> LimitPoints;
	FIntPoint LocalStartingPoint(-1,-1);

	RiverShapeVertex.Append(GetBorderSection(ShapeMap, SpreadPoint, StartingPoint));
	int index = 0;

	while(RiverShapeVertex.Last() != StartingPoint && LocalStartingPoint != StartingPoint && index < 10) {
		UE_LOG(LogTemp, Warning, TEXT("WE HAVEN'T FINISH IT YET ;)"));

		LimitPoints = GetBorderFlood(FloodMap, RiverShapeVertex.Last());

		LocalStartingPoint = LimitPoints.Last();

		if(LimitPoints.Num() > 1)
			for(size_t j = 0; j < LimitPoints.Num() - 1; j++) {
				RiverShapeVertex.Add(LimitPoints[j]);
			}


		if(LocalStartingPoint == StartingPoint)
			continue;

		SpreadPoint = BorderPointsInOrder(ShapeMap, LocalStartingPoint, { LocalStartingPoint })[0];

		UE_LOG(LogTemp, Warning, TEXT("Next spread point point: %d x %d"), SpreadPoint.X, SpreadPoint.Y);

		RiverShapeVertex.Append(GetBorderSection(ShapeMap, SpreadPoint, LocalStartingPoint));
		//GetBorderFlood(FloodMap, RiverShapeVertex.Last());
		index++;

		UE_LOG(LogTemp, Warning, TEXT("Last was: %d x %d and Start was: %d x %d"), RiverShapeVertex.Last().X, RiverShapeVertex.Last().Y,
			   StartingPoint.X, StartingPoint.Y);

	}

	UE_LOG(LogTemp, Warning, TEXT("LAST INDEX WAS: %d"), index);
	UE_LOG(LogTemp, Warning, TEXT(" "));

	/*for(FIntPoint BorderPoint : RiverShapeVertex)
		FloodMap.setItem(1, BorderPoint.X, BorderPoint);*/

	/*if(BordersPoints.Num() == 2) {
		UE_LOG(LogTemp, Warning, TEXT("Point 1: %d x %d"), BordersPoints[0].X, BordersPoints[0].Y);
		UE_LOG(LogTemp, Warning, TEXT("Point 2: %d x %d"), BordersPoints[1].X, BordersPoints[1].Y);
	}*/



	return RiverShapeVertex;

	//Try to simplify the vertices

}

TArray<FIntPoint> TerrainAlgorithms::GetBorderFlood(const MArray<float>& FloodMap, FIntPoint StartPoint) {
	FIntPoint NextPoint;
	TArray<FIntPoint> PassingPoints;
	FIntVector2 ArraySize = FIntVector2(FloodMap.GetArraySize().X, FloodMap.GetArraySize().Y);

	//int8 orientation = -1;	
	int8 direction = 0;
	int8 side = 0;

	/*if(StartPoint.X == 0 || StartPoint.X == ArraySize.X - 1)
		orientation = 1;

	if(StartPoint.Y == 0 || StartPoint.Y == ArraySize.Y - 1)
		orientation = 0;*/

	TMap<TTuple<int8, int8>, TTuple<int8, int8>> RotationTable;

	RotationTable.Add(TTuple<int8, int8>(1, 2), TTuple<int8, int8>(4, 1));
	RotationTable.Add(TTuple<int8, int8>(1, 4), TTuple<int8, int8>(3, 1));
	RotationTable.Add(TTuple<int8, int8>(2, 2), TTuple<int8, int8>(4, 3));
	RotationTable.Add(TTuple<int8, int8>(2, 4), TTuple<int8, int8>(3, 3));
	RotationTable.Add(TTuple<int8, int8>(3, 1), TTuple<int8, int8>(2, 2));
	RotationTable.Add(TTuple<int8, int8>(3, 3), TTuple<int8, int8>(1, 2));
	RotationTable.Add(TTuple<int8, int8>(4, 1), TTuple<int8, int8>(2, 4));
	RotationTable.Add(TTuple<int8, int8>(4, 3), TTuple<int8, int8>(1, 4));

	//RotationTable

	if((StartPoint.X == 0 || StartPoint.X == ArraySize.X - 1) && StartPoint.Y - 1 > 0 && FloodMap.getItem(StartPoint.X, StartPoint.Y - 1) == -1) {
		direction = 1; // UP / Decrease
		side = StartPoint.X == 0 ? 2 : 4; // LEFT or RIGHT side
	}

	if((StartPoint.X == 0 || StartPoint.X == ArraySize.X - 1) && StartPoint.Y + 1 < (ArraySize.Y - 1) && FloodMap.getItem(StartPoint.X, StartPoint.Y + 1) == -1) {
		direction = 2; // DOWN / Increase
		side = StartPoint.X == 0 ? 2 : 4; // LEFT or RIGHT side
	}

	if((StartPoint.Y == 0 || StartPoint.Y == ArraySize.Y - 1) && StartPoint.X - 1 > 0 && FloodMap.getItem(StartPoint.X - 1, StartPoint.Y) == -1) {
		direction = 3; // LEFT / Decrease
		side = StartPoint.Y == 0 ? 1 : 3; // TOP or BOTTOM side
	}

	if((StartPoint.Y == 0 || StartPoint.Y == ArraySize.Y - 1) && StartPoint.X + 1 < (ArraySize.X - 1) && FloodMap.getItem(StartPoint.X + 1, StartPoint.Y) == -1) {
		direction = 4; // RIGHT / Increase
		side = StartPoint.Y == 0 ? 1 : 3; // TOP or BOTTOM side
	}

	bool FoundBorder = false;
	FIntPoint ExploringPoint = StartPoint;
	FIntPoint CornerPoint;
	int start, end;
	while(!FoundBorder) {

		UE_LOG(LogTemp, Warning, TEXT("Current direction: %d and side: %d"), direction, side);


		if(direction % 2 == 0) {					// If direction is DOWN or RIGHT, where i++

			if(side % 2 == 0) {                     // DOWN Cases
				start = ExploringPoint.Y + 1;
				end = ArraySize.Y - 1;

				CornerPoint = FIntPoint(ExploringPoint.X, ArraySize.Y - 1);

				for(int i = start; i < end; i++) {
					if(FloodMap.getItem(ExploringPoint.X, i) != -1) {
						NextPoint = FIntPoint(ExploringPoint.X, i);
						FoundBorder = true;
						break;
					}
				}

			} else {								// RIGHT Cases
				start = ExploringPoint.X + 1;
				end = ArraySize.X - 1;

				CornerPoint = FIntPoint(ArraySize.X - 1, ExploringPoint.Y);

				for(int i = start; i < end; i++) {
					if(FloodMap.getItem(i, ExploringPoint.Y) != -1) {
						NextPoint = FIntPoint(i, ExploringPoint.Y);
						FoundBorder = true;
						break;
					}
				}
			}

		} else {									// If direction is UP or LEFT, where i--

			if(side % 2 == 0) {						// UP Cases
				start = ExploringPoint.Y - 1;
				end = 0;

				CornerPoint = FIntPoint(ExploringPoint.X, 0);

				for(int i = start; i >= end; i--) {
					if(FloodMap.getItem(ExploringPoint.X, i) != -1) {
						NextPoint = FIntPoint(ExploringPoint.X, i);
						FoundBorder = true;
						break;
					}
				}

			} else {								// LEFT Cases
				start = ExploringPoint.X - 1;
				end = 0;

				CornerPoint = FIntPoint(0, ExploringPoint.Y);

				for(int i = start; i >= end; i--) {
					if(FloodMap.getItem(i, ExploringPoint.Y) != -1) {
						NextPoint = FIntPoint(i, ExploringPoint.Y);
						FoundBorder = true;
						break;
					}
				}
			}

		}


		//If we have reach a corner, this get the new direction to keep searching in that direction
		TTuple<int8, int8> NewRotation = RotationTable[TTuple<int8, int8>(direction, side)];
		direction = NewRotation.Key;
		side = NewRotation.Value;

		//New starting point, the corner we hit
		ExploringPoint = CornerPoint;

		//We add the corner so we can do the mesh later
		PassingPoints.Add(CornerPoint);

		if(!FoundBorder)
			UE_LOG(LogTemp, Warning, TEXT("Next corner: %d x %d"), CornerPoint.X, CornerPoint.Y);


	}

	UE_LOG(LogTemp, Warning, TEXT("Next point: %d x %d"), NextPoint.X, NextPoint.Y);

	PassingPoints.Add(NextPoint);

	return PassingPoints;
}

TArray<FIntPoint> TerrainAlgorithms::GetBorderSection(const MArray<int8>& ShapeMap, FIntPoint StartPoint, FIntPoint PreviousPoint) {

	TArray<FIntPoint> BordersPoints;
	TArray<FIntPoint> PointsInOrder = { PreviousPoint , StartPoint };
	TArray<FIntPoint> PreviousPoints;


	BordersPoints = BorderPointsInOrder(ShapeMap, StartPoint, { PreviousPoint });
	bool BorderComplete = false;

	while(BordersPoints.Num() >= 1 && BordersPoints.Num() <= 2 && !BorderComplete) {
		//UE_LOG(LogTemp, Warning, TEXT("The next point is: %d x %d"), BordersPoints[0].X, BordersPoints[0].Y);
		
	if(BordersPoints.Num() == 2) {
			PointsInOrder.Add(BordersPoints[0]);
			PointsInOrder.Add(BordersPoints[1]);

			//PreviousPoint = BordersPoints[0];
			PreviousPoints = { BordersPoints[0], StartPoint };
			StartPoint = BordersPoints[1];

			

		} else {
			PointsInOrder.Add(BordersPoints[0]);
			PreviousPoints = { StartPoint };
			StartPoint = BordersPoints[0];
		}



		
	BordersPoints = BorderPointsInOrder(ShapeMap, StartPoint, { PreviousPoints });
	}

	//If you got this error, it means the river it is doing a weird shape and two borders are really close to each other
	//I think it is solvable, but also kinda rare to happens, but if it does, good luck! Shouldn't be difficult to fix ;)
	if(BordersPoints.Num() > 2) {
		UE_LOG(LogTemp, Warning, TEXT("FAILING AT START POINT: %d x %d"), StartPoint.X, StartPoint.Y);
		UE_LOG(LogTemp, Warning, TEXT("WITH PREVIOUS POINT: %d x %d"), PreviousPoint.X, PreviousPoint.Y);

		for(FIntPoint points : BordersPoints) {
			UE_LOG(LogTemp, Warning, TEXT("POINT: %d x %d"), points.X, points.Y);
		}

	}

	UE_LOG(LogTemp, Warning, TEXT("Point in order added: %d"), PointsInOrder.Num());
	UE_LOG(LogTemp, Warning, TEXT("Temp point size: %d"), BordersPoints.Num());
	UE_LOG(LogTemp, Warning, TEXT("Last point added: %d x %d"), StartPoint.X, StartPoint.Y);

	return PointsInOrder;

}

TArray<FIntPoint> TerrainAlgorithms::BorderPointsInOrder(const MArray<int8>& ShapeMap, FIntPoint StartPoint, TArray<FIntPoint> PreviousPoint) {
	TArray<FIntPoint> ResultPoints;
	FIntPoint ArraySize = FIntPoint(ShapeMap.GetArraySize().X, ShapeMap.GetArraySize().Y);

	//FIntPoint Previous = PreviousPoint;
	FIntPoint Current = StartPoint;

	MArray<int8> Matrix(-2, 3, 3);
	int offsetx = -1;
	int offsety = 0;

	for(int32 y = Current.Y - 1; y < Current.Y + 2; y++) {
		for(int32 x = Current.X - 1; x < Current.X + 2; x++) {
			offsetx++;
			if(y < 0 || x < 0) { // Out of bounds
				Matrix.setItem(-2, offsetx, offsety);
				continue;
			}
			if(y >= ArraySize.Y || x >= ArraySize.X) { // Out of bounds
				Matrix.setItem(-2, offsetx, offsety);
				continue;
			}
			if(y == Current.Y && x == Current.X) { //Same point as the original
				Matrix.setItem(-2, offsetx, offsety);
				continue;
			}

			Matrix.setItem(ShapeMap.getItem(x, y), offsetx, offsety);
			//Heights.Add(FIntPoint(x, y));

		}
		offsety++;
		offsetx = -1;
	}

	int8 invalid = 0;
	int8 border = 0;
	int8 flood = 0;
	int8 outside = 0;

	for(uint32 y = 0; y < 3; y++) {
		for(uint32 x = 0; x < 3; x++) {
			switch(Matrix.getItem(x, y)) {
				case -2:
					invalid++;
					break;

				case -1:
					flood++;
					break;

				case 0:
					outside++;
					break;

				case 1:
					FIntPoint BorderCheck = FIntPoint(Current.X + x - 1, Current.Y + y - 1);
					if(!PreviousPoint.Contains(BorderCheck))
						ResultPoints.Add(BorderCheck);
					border++;
					break;

				default:
					break;
			}

		}
	}

	if(ResultPoints.Num() == 2 && !PreviousPoint.Contains(StartPoint)) {
		UE_LOG(LogTemp, Warning, TEXT("Point: %d x %d"), StartPoint.X, StartPoint.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Previous: %d x %d"), PreviousPoint.X, PreviousPoint.Y);
		UE_LOG(LogTemp, Warning, TEXT("Invalids: %d, Outside: %d, Border: %d, Flood: %d"), invalid, outside, border, flood);
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 0), Matrix.getItem(1, 0), Matrix.getItem(2, 0));
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 1), Matrix.getItem(1, 1), Matrix.getItem(2, 1));
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 2), Matrix.getItem(1, 2), Matrix.getItem(2, 2));

		//To maintain the contiunity we are gonna asume, the closes one (in the cardinal points) is always closer
		if(ResultPoints[1].X == StartPoint.X || ResultPoints[1].Y == StartPoint.Y) {
			ResultPoints.Swap(0, 1);
		}

		for(FIntPoint result : ResultPoints) {
			UE_LOG(LogTemp, Warning, TEXT("Result: %d x %d"), result.X, result.Y);
		}

	}

	if(ResultPoints.Num() > 2) {
		UE_LOG(LogTemp, Warning, TEXT("HUMONGOUS"));
		UE_LOG(LogTemp, Warning, TEXT("Point: %d x %d"), StartPoint.X, StartPoint.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Previous: %d x %d"), PreviousPoint.X, PreviousPoint.Y);
		UE_LOG(LogTemp, Warning, TEXT("Invalids: %d, Outside: %d, Border: %d, Flood: %d"), invalid, outside, border, flood);
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 0), Matrix.getItem(1, 0), Matrix.getItem(2, 0));
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 1), Matrix.getItem(1, 1), Matrix.getItem(2, 1));
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 2), Matrix.getItem(1, 2), Matrix.getItem(2, 2));
	}

	return ResultPoints;
}

MArray<float> TerrainAlgorithms::CalculateWaterAreasSimplified(const MArray <float>& HeightMap) {
	HeightMapVariables HMVariables;
	CalculateVariables(HeightMap, HMVariables);

	UE_LOG(LogTemp, Warning, TEXT("Min: %f Max: %f"), HMVariables.MinValue, HMVariables.MaxValue);
	UE_LOG(LogTemp, Warning, TEXT("Mean: %f"), HMVariables.Mean);
	UE_LOG(LogTemp, Warning, TEXT("Standard deviation: %lf"), HMVariables.StandardDeviation);

	//This uses the Mean and SD, which can be a bad heuristic for this case, if the Curve is changed drastically, this can lead to unwanted results
	float FloodStartThreshold = HMVariables.Mean - HMVariables.StandardDeviation;

	FUintVector2 ArraySize = HeightMap.GetArraySize();
	MArray<float> FloodMap(0, ArraySize.X, ArraySize.Y);
	MArray<float> ShapeMap(0, ArraySize.X, ArraySize.Y);

	//bool CurrentPoint = (HeightMap.getItem(0, 0) < FloodStartThreshold) ? true : false;

	// First pass to get the shapes
	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {

			if(HeightMap.getItem(x, y) < FloodStartThreshold)
				FloodMap.setItem(-1, x, y);
			//if(isUnder != CurrentPoint) {
			//	FloodMap.setItem(1, x, y);
			//	CurrentPoint = !CurrentPoint;
			//	line = true;
			//}
		}
	}

	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {

			if(FloodMap.getItem(x, y) == -1)
				ShapeMap.setItem(-1, x, y);

			// Top cell
			if(y > 0 && FloodMap.getItem(x, y) != FloodMap.getItem(x, y - 1)) {
				ShapeMap.setItem(1, x, y);
			}
			// Bottom cell
			if(y < ArraySize.Y - 1 && FloodMap.getItem(x, y) != FloodMap.getItem(x, y + 1)) {
				ShapeMap.setItem(1, x, y);
			}
			// Left cell
			if(x > 0 && FloodMap.getItem(x, y) != FloodMap.getItem(x - 1, y)) {
				ShapeMap.setItem(1, x, y);
			}
			// Right cell
			if(x < ArraySize.X - 1 && FloodMap.getItem(x, y) != FloodMap.getItem(x + 1, y)) {
				ShapeMap.setItem(1, x, y);
			}

		}
	}

	// Top and bottom borders
	for(uint32 x = 0; x < ArraySize.X; x++) {
		if(FloodMap.getItem(x, 0) == -1)
			ShapeMap.setItem(1, x, 0);

		if(FloodMap.getItem(x, ArraySize.Y - 1) == -1)
			ShapeMap.setItem(1, x, ArraySize.Y - 1);
	}

	// Right and left Border
	for(uint32 y = 0; y < ArraySize.Y; y++) {
		if(FloodMap.getItem(0, y) == -1)
			ShapeMap.setItem(1, 0, y);

		if(FloodMap.getItem(ArraySize.X - 1, y) == -1)
			ShapeMap.setItem(1, ArraySize.X - 1, y);
	}

	TArray<TSet<FIntPoint>> RiverShapes;

	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {

		}
	}

	CalculateRiverShape(ShapeMap, FIntPoint(0, 0));

	return ShapeMap;
}

TSet<FIntPoint> TerrainAlgorithms::CalculateRiverShape(const MArray<float>& ShapeMap, FIntPoint StartPoint) {

	FIntPoint ArraySize = FIntPoint(ShapeMap.GetArraySize().X, ShapeMap.GetArraySize().Y);

	TSet<FIntPoint> RiverShape;
	RiverShape.Add(StartPoint);

	FIntPoint Previous = StartPoint;
	FIntPoint Current = StartPoint;

	//TArray<FIntPoint> NextPoint = GetSurroundingHeights(ShapeMap, StartPoint);

	MArray<int8> Matrix(-2, 3, 3);
	int offsetx = 0;
	int offsety = 0;

	for(int32 y = Current.Y - 1; y < Current.Y + 2; y++) {
		for(int32 x = Current.X - 1; x < Current.X + 2; x++) {
			if(y < 0 || x < 0) { // Out of bounds
				Matrix.setItem(-2, offsetx, offsety);
				continue;
			}
			if(y >= ArraySize.Y || x >= ArraySize.X) { // Out of bounds
				Matrix.setItem(-2, offsetx, offsety);
				continue;
			}
			if(y == Current.Y && x == Current.X) { //Same point as the original
				Matrix.setItem(-2, offsetx, offsety);
				continue;
			}

			Matrix.setItem(ShapeMap.getItem(x, y), offsetx, offsety);
			//Heights.Add(FIntPoint(x, y));
			offsetx++;
		}
		offsety++;
		offsetx = 0;
	}

	int8 invalid = 0;
	int8 border = 0;
	int8 flood = 0;
	int8 outside = 0;

	for(uint32 y = 0; y < 3; y++) {
		for(uint32 x = 0; x < 3; x++) {
			switch(Matrix.getItem(x, y)) {
				case -2:
					invalid++;
					break;

				case -1:
					flood++;
					break;

				case 0:
					outside++;
					break;

				case 1:


					border++;
					break;

				default:
					break;
			}

		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Invalids: %d, Outside: %d, Border: %d, Flood: %d"), invalid, outside, border, flood);
	if(border > 2) {
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 0), Matrix.getItem(1, 0), Matrix.getItem(2, 0));
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 1), Matrix.getItem(1, 1), Matrix.getItem(2, 1));
		UE_LOG(LogTemp, Warning, TEXT("| %d | %d | %d |"), Matrix.getItem(0, 2), Matrix.getItem(1, 2), Matrix.getItem(2, 2));
	}


	return RiverShape;
}

void TerrainAlgorithms::Flood(const MArray<float>& HeightMap, MArray<float>& FloodMap, MArray<bool>& CheckedAreas, FIntPoint FloodStartPoint, float FloodThreshold) {

	TArray<FIntPoint> Flooding;
	Flooding.Add(FloodStartPoint);
	//CheckedAreas.setItem(true, FloodStartPoint.X, FloodStartPoint.Y);

	//TSet<FIntPoint> Flood2;

	while(!Flooding.IsEmpty()) {


		FIntPoint CurrentPoint = Flooding.Pop();
		//UE_LOG(LogTemp, Warning, TEXT("Skipping %dx%d"), CurrentPoint.X, CurrentPoint.Y);

		float PointHeight = HeightMap.getItem(CurrentPoint.X, CurrentPoint.Y);
		if(PointHeight <= FloodThreshold) {
			//For testing, flood area is gonna be -1 and dry areas 0, mean to be changed in the future
			FloodMap.setItem(-1, CurrentPoint.X, CurrentPoint.Y);
		} else {
			FloodMap.setItem(0, CurrentPoint.X, CurrentPoint.Y);
		}


		TArray<FIntPoint> SurroundingPoints = GetSurroundingHeights(HeightMap, CurrentPoint);

		for(size_t i = 0; i < SurroundingPoints.Num(); i++) {

			float SurroundingPointHeight = HeightMap.getItem(SurroundingPoints[i].X, SurroundingPoints[i].Y);

			if(CheckedAreas.getItem(SurroundingPoints[i].X, SurroundingPoints[i].Y))
				continue;

			//If this point is lower than the reference, we start flooding from it too
			if(SurroundingPointHeight <= FloodThreshold)
				Flooding.Add(SurroundingPoints[i]);

			CheckedAreas.setItem(true, SurroundingPoints[i].X, SurroundingPoints[i].Y);
		}

	}

	//UE_LOG(LogTemp, Warning, TEXT("Array Size: %d"), Flooding.Num());

}

TArray<FIntPoint> TerrainAlgorithms::GetSurroundingHeights(const MArray<float>& HeightMap, FIntPoint BasePoint) {

	FIntPoint ArraySize = FIntPoint(HeightMap.GetArraySize().X, HeightMap.GetArraySize().Y);

	TArray<FIntPoint> Heights;

	for(int32 y = BasePoint.Y - 1; y < BasePoint.Y + 2; y++) {
		for(int32 x = BasePoint.X - 1; x < BasePoint.X + 2; x++) {
			if(y < 0 || x < 0) // Out of bounds
				continue;
			if(y >= ArraySize.Y || x >= ArraySize.X) // Out of bounds
				continue;
			if(y == BasePoint.Y && x == BasePoint.X) //Same point as the original
				continue;
			Heights.Add(FIntPoint(x, y));
		}
	}

	return Heights;

}

void TerrainAlgorithms::CalculateVariables(const MArray<float>& HeightMap, HeightMapVariables& Variables) {

	FUintVector2 ArraySize = HeightMap.GetArraySize();

	float MaxValue = -std::numeric_limits<float>::infinity();
	float MinValue = std::numeric_limits<float>::infinity();


	float sum = 0;

	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {
			//Max calculation
			if(HeightMap.getItem(x, y) > MaxValue)
				MaxValue = HeightMap.getItem(x, y);

			//Min Calculation
			if(HeightMap.getItem(x, y) < MinValue)
				MinValue = HeightMap.getItem(x, y);

			//Mean Calculation
			sum += HeightMap.getItem(x, y);
		}
	}

	uint32 size = (ArraySize.Y * ArraySize.X);
	float mean = sum / size;

	float sumSD = 0;
	for(uint32 y = 0; y < ArraySize.Y; y++) {
		for(uint32 x = 0; x < ArraySize.X; x++) {
			sumSD += FMath::Pow(HeightMap.getItem(x, y) - mean, 2);
		}
	}

	double variance = sumSD / size;
	double SD = FMath::Sqrt(variance);

	Variables.MaxValue = MaxValue;
	Variables.MinValue = MinValue;
	Variables.Mean = mean;
	Variables.StandardDeviation = SD;

}