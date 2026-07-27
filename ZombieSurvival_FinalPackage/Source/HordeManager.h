// HordeManager.h
// Source/ZombieSurvival/AI/HordeManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HordeManager.generated.h"

USTRUCT()
struct FHordeGroup
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TWeakObjectPtr<AZombieBase>> Members;

    UPROPERTY()
    FVector TargetLocation;

    UPROPERTY()
    float SpawnTime = 0.0f;

    UPROPERTY()
    bool bIsActive = false;
};

UCLASS()
class ZOMBIESURVIVAL_API AHordeManager : public AActor
{
    GENERATED_BODY()

public:
    AHordeManager();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horde")
    int32 MaxHordeSize = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horde")
    float HordeSpawnInterval = 300.0f; // co 5 min

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horde")
    float HordeSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horde")
    TSubclassOf<AZombieBase> ZombieClass;

    UFUNCTION(BlueprintCallable, Category = "Horde")
    void TriggerHorde(FVector TargetLocation, int32 Size);

    UFUNCTION(BlueprintCallable, Category = "Horde")
    void SpawnHorde();

protected:
    UPROPERTY()
    TArray<FHordeGroup> ActiveHordes;

    float LastHordeTime = 0.0f;

    UFUNCTION()
    FVector FindHordeSpawnLocation(FVector Target) const;
};
