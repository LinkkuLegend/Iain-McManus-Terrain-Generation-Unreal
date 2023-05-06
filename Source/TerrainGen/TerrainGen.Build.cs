// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TerrainGen : ModuleRules {
    public TerrainGen(ReadOnlyTargetRules Target) : base(Target) {
       // bLegacyPublicIncludePaths = false;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

       // PublicIncludePaths.Add("TerrainGen");

        PublicDependencyModuleNames.AddRange(new string[] { "Core","CoreUObject","Engine","InputCore","EnhancedInput","ProceduralMeshComponent",
        "MeshDescription","StaticMeshDescription","MeshConversion","RealtimeMeshComponent"});

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
