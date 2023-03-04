// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TerrainGen : ModuleRules {
    public TerrainGen(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core","CoreUObject","Engine","InputCore","HeadMountedDisplay","EnhancedInput","ProceduralMeshComponent","GeometryFramework","GeometryCore","DynamicMesh"});
    }
}
