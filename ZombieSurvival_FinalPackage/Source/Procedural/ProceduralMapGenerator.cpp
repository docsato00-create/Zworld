// ProceduralMapGenerator.cpp

#include "ProceduralMapGenerator.h"
#include "WreckedVehicle.h"
#include "LootContainer.h"
#include "ZombieSpawner.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

AProceduralMapGenerator::AProceduralMapGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AProceduralMapGenerator::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority && Seed == 0)
    {
        Seed = FMath::RandRange(1, 999999);
    }

    if (GetLocalRole() == ROLE_Authority)
    {
        GenerateMap();
    }
}

void AProceduralMapGenerator::GenerateMap()
{
    ClearMap();
    GenerateTerrain();
    SpawnPOIs();
    SpawnWreckedVehicles();
    SpawnLootContainers();
    SpawnZombieSpawners();
}

void AProceduralMapGenerator::ClearMap()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
    SpawnedActors.Empty();
    SpawnedPOILocations.Empty();
}

void AProceduralMapGenerator::GenerateTerrain()
{
    // Terrain generation using landscape or mesh
    // For MVP: Use existing landscape and modify heights
    // Full implementation would use CustomMeshComponent or ProceduralMeshComponent
}

void AProceduralMapGenerator::SpawnPOIs()
{
    for (const FPOIConfig& POI : PointsOfInterest)
    {
        int32 Count = FMath::RandRange(POI.MinCount, POI.MaxCount);
        int32 Attempts = 0;
        int32 Spawned = 0;

        while (Spawned < Count && Attempts < 50)
        {
            Attempts++;
            FVector Location = FindValidSpawnLocation(500.0f, MapSize / 2.0f);

            bool bTooClose = false;
            for (const FVector& Existing : SpawnedPOILocations)
            {
                if (FVector::Dist(Location, Existing) < POI.MinDistanceFromOthers)
                {
                    bTooClose = true;
                    break;
                }
            }

            if (!bTooClose && POI.POIActorClass)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* NewPOI = GetWorld()->SpawnActor<AActor>(POI.POIActorClass, Location, FRotator::ZeroRotator, Params);
                if (NewPOI)
                {
                    SpawnedActors.Add(NewPOI);
                    SpawnedPOILocations.Add(Location);
                    Spawned++;
                }
            }
        }
    }
}

void AProceduralMapGenerator::SpawnWreckedVehicles()
{
    for (int32 i = 0; i < MaxWreckedVehicles; i++)
    {
        FVector Location = FindValidSpawnLocation(200.0f, MapSize / 2.0f);

        if (WreckedVehicleClass)
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            FRotator Rotation = FRotator(0, FMath::RandRange(0, 360), 0);
            AWreckedVehicle* Vehicle = GetWorld()->SpawnActor<AWreckedVehicle>(WreckedVehicleClass, Location, Rotation, Params);
            if (Vehicle)
            {
                SpawnedActors.Add(Vehicle);
            }
        }
    }
}

void AProceduralMapGenerator::SpawnLootContainers()
{
    // Spawn containers near POIs and randomly
    for (const FVector& POILoc : SpawnedPOILocations)
    {
        int32 ContainerCount = FMath::RandRange(3, 8);
        for (int32 i = 0; i < ContainerCount; i++)
        {
            FVector Offset = FVector(FMath::RandRange(-300, 300), FMath::RandRange(-300, 300), 0);
            FVector Location = POILoc + Offset;

            if (LootContainerClass)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                ALootContainer* Container = GetWorld()->SpawnActor<ALootContainer>(LootContainerClass, Location, FRotator::ZeroRotator, Params);
                if (Container)
                {
                    SpawnedActors.Add(Container);
                }
            }
        }
    }
}

void AProceduralMapGenerator::SpawnZombieSpawners()
{
    // Spawn spawners around the map
    int32 SpawnerCount = MapSize / 500; // Density based on map size

    for (int32 i = 0; i < SpawnerCount; i++)
    {
        FVector Location = FindValidSpawnLocation(500.0f, MapSize / 2.0f);

        // Spawn spawner volume
        // Implementation depends on ZombieSpawner class
    }
}

float AProceduralMapGenerator::GetHeightAtLocation(FVector2D Location) const
{
    return GetNoiseValue(Location.X, Location.Y) * HeightMultiplier;
}

FName AProceduralMapGenerator::GetBiomeAtLocation(FVector2D Location) const
{
    float Height = GetNoiseValue(Location.X, Location.Y);
    float Temp = GetNoiseValue(Location.X + 1000, Location.Y + 1000);
    float Moist = GetNoiseValue(Location.X + 2000, Location.Y + 2000);

    for (const FBiomeConfig& Biome : Biomes)
    {
        if (Height >= Biome.MinHeight && Height <= Biome.MaxHeight &&
            FMath::Abs(Temp - Biome.Temperature) < 0.2f &&
            FMath::Abs(Moist - Biome.Moisture) < 0.2f)
        {
            return Biome.BiomeName;
        }
    }

    return FName("Meadows");
}

float AProceduralMapGenerator::GetNoiseValue(float X, float Y) const
{
    // Simple pseudo-random noise (replace with Perlin/Simplex noise library)
    float NX = X * NoiseScale + Seed;
    float NY = Y * NoiseScale + Seed;
    return FMath::PerlinNoise2D(FVector2D(NX, NY)) * 0.5f + 0.5f;
}

FVector AProceduralMapGenerator::FindValidSpawnLocation(float MinRadius, float MaxRadius) const
{
    float Angle = FMath::RandRange(0.0f, 2.0f * PI);
    float Radius = FMath::RandRange(MinRadius, MaxRadius);

    FVector Location;
    Location.X = FMath::Cos(Angle) * Radius;
    Location.Y = FMath::Sin(Angle) * Radius;
    Location.Z = GetHeightAtLocation(FVector2D(Location.X, Location.Y));

    return Location;
}

bool AProceduralMapGenerator::IsLocationValid(FVector Location) const
{
    // Check if location is on navmesh
    FNavLocation NavLocation;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        return NavSys->ProjectPointToNavigation(Location, NavLocation);
    }
    return true;
}
