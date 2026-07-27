// SurvivalTypes.h
// Umieść w: Source/ZombieSurvival/Core/SurvivalTypes.h

#pragma once

#include "CoreMinimal.h"
#include "SurvivalTypes.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EZombieType : uint8
{
    Walker      UMETA(DisplayName = "Walker"),
    Runner      UMETA(DisplayName = "Runner"),
    Tank        UMETA(DisplayName = "Tank"),
    Spitter     UMETA(DisplayName = "Spitter"),
    Boss        UMETA(DisplayName = "Boss")
};

UENUM(BlueprintType)
enum class EBuildingTier : uint8
{
    Wood        UMETA(DisplayName = "Wood"),
    Stone       UMETA(DisplayName = "Stone"),
    Metal       UMETA(DisplayName = "Metal"),
    Armored     UMETA(DisplayName = "Armored")
};

USTRUCT(BlueprintType)
struct FSurvivalStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float MaxHunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float CurrentHunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float MaxThirst = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float CurrentThirst = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float CurrentStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float HungerDecayPerMinute = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float ThirstDecayPerMinute = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float StaminaRegenPerSecond = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float StaminaDrainPerSecond = 10.0f;
};

USTRUCT(BlueprintType)
struct FVehiclePartSlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SlotName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class UItemData> RequiredItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsOccupied = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Condition = 100.0f;
};
