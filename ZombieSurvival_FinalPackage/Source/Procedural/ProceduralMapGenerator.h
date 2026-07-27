// ProceduralMapGenerator.h
// Source/ZombieSurvival/Procedural/ProceduralMapGenerator.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMapGenerator.generated.h"

USTRUCT(BlueprintType)
struct FBiomeConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BiomeName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinHeight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHeight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Temperature = 0.5f; // 0=cold, 1=hot

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Moisture = 0.5f; // 0=dry, 1=wet

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<class AActor>> SpawnableActors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<class AZombieSpawner>> ZombieSpawners;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 LootContainerDensity = 10;
};

USTRUCT(BlueprintType)
struct FPOIConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName POIName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class AActor> POIActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinDistanceFromOthers = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> AllowedBiomes;
};

UCLASS()
class ZOMBIESURVIVAL_API AProceduralMapGenerator : public AActor
{
    GENERATED_BODY()

public:
    AProceduralMapGenerator();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    int32 MapSize = 4000; // 4km x 4km

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    int32 ChunkSize = 500;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    float NoiseScale = 0.001f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    float HeightMultiplier = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    TArray<FBiomeConfig> Biomes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    TArray<FPOIConfig> PointsOfInterest;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    int32 MaxWreckedVehicles = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    TSubclassOf<class AWreckedVehicle> WreckedVehicleClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Gen")
    TSubclassOf<class ALootContainer> LootContainerClass;

    // === GENERATION ===
    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void GenerateMap();

    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void ClearMap();

    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void GenerateTerrain();

    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void SpawnPOIs();

    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void SpawnWreckedVehicles();

    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void SpawnLootContainers();

    UFUNCTION(BlueprintCallable, Category = "Map Gen")
    void SpawnZombieSpawners();

    UFUNCTION(BlueprintPure, Category = "Map Gen")
    float GetHeightAtLocation(FVector2D Location) const;

    UFUNCTION(BlueprintPure, Category = "Map Gen")
    FName GetBiomeAtLocation(FVector2D Location) const;

protected:
    UPROPERTY()
    TArray<FVector> SpawnedPOILocations;

    UPROPERTY()
    TArray<AActor*> SpawnedActors;

    UFUNCTION()
    float GetNoiseValue(float X, float Y) const;

    UFUNCTION()
    FVector FindValidSpawnLocation(float MinRadius, float MaxRadius) const;

    UFUNCTION()
    bool IsLocationValid(FVector Location) const;
};
