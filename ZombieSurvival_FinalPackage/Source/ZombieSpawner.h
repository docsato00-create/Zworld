// ZombieSpawner.h
// Source/ZombieSurvival/AI/ZombieSpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "ZombieSpawner.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AZombieSpawner : public AVolume
{
    GENERATED_BODY()

public:
    AZombieSpawner();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // === SPAWN CONFIG ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    int32 MaxZombies = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnInterval = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnRadius = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    TArray<TSubclassOf<class AZombieBase>> ZombieTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float DaySpawnMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float NightSpawnMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    bool bOnlySpawnAtNight = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    bool bSpawnDuringHorde = true;

    // === RUNTIME ===
    UPROPERTY(BlueprintReadOnly, Category = "Spawn")
    TArray<TWeakObjectPtr<AZombieBase>> SpawnedZombies;

    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void TrySpawn();

    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void ClearAllZombies();

    UFUNCTION(BlueprintPure, Category = "Spawn")
    int32 GetAliveZombieCount() const;

protected:
    UPROPERTY()
    class ADayNightManager* DayNightManager;

    UPROPERTY()
    FTimerHandle SpawnTimerHandle;

    UFUNCTION()
    FVector FindSpawnLocation() const;

    UFUNCTION()
    TSubclassOf<AZombieBase> SelectZombieType() const;

    UFUNCTION()
    float GetCurrentSpawnMultiplier() const;
};
