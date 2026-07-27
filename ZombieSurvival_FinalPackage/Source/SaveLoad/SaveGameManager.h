// SaveGameManager.h
// Source/ZombieSurvival/SaveLoad/SaveGameManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameManager.generated.h"

USTRUCT(BlueprintType)
struct FBuildingSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FString BuildingClassPath;

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    UPROPERTY()
    float Health;

    UPROPERTY()
    int32 UpgradeLevel;

    UPROPERTY()
    FString OwnerPlayerID;
};

USTRUCT(BlueprintType)
struct FVehicleSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FString VehicleClassPath;

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    UPROPERTY()
    float Fuel;

    UPROPERTY()
    float Health;

    UPROPERTY()
    FString OwnerPlayerID;
};

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FString PlayerName;

    UPROPERTY()
    FString SteamID;

    UPROPERTY()
    int32 Level;

    UPROPERTY()
    float XP;

    UPROPERTY()
    int32 SkillPoints;

    UPROPERTY()
    FVector LastLocation;

    UPROPERTY()
    FSurvivalStats Stats;

    UPROPERTY()
    TArray<FInventorySlot> Inventory;

    UPROPERTY()
    TArray<FString> LearnedRecipes;

    UPROPERTY()
    int32 ZombiesKilled;

    UPROPERTY()
    int32 PlayersKilled;

    UPROPERTY()
    float TimePlayed;
};

USTRUCT(BlueprintType)
struct FWorldSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FString MapName;

    UPROPERTY()
    int32 CurrentDay;

    UPROPERTY()
    float TimeOfDay;

    UPROPERTY()
    int32 Seed;

    UPROPERTY()
    TArray<FBuildingSaveData> Buildings;

    UPROPERTY()
    TArray<FVehicleSaveData> Vehicles;

    UPROPERTY()
    TArray<FPlayerSaveData> Players;

    UPROPERTY()
    FString SaveDate;

    UPROPERTY()
    FString Version;
};

UCLASS()
class ZOMBIESURVIVAL_API USurvivalSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FWorldSaveData WorldData;
};

UCLASS()
class ZOMBIESURVIVAL_API ASaveGameManager : public AActor
{
    GENERATED_BODY()

public:
    ASaveGameManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    float AutoSaveInterval = 300.0f; // 5 min

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    int32 MaxSaveSlots = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    FString SaveSlotPrefix = TEXT("ZombieSurvival_Save");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    FString CurrentVersion = TEXT("0.1.0");

    // === SAVE ===
    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveWorld(int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadWorld(int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void AutoSave();

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SavePlayerData(APlayerState* PlayerState);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadPlayerData(APlayerState* PlayerState);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void DeleteSave(int32 SlotIndex);

    UFUNCTION(BlueprintPure, Category = "Save")
    bool DoesSaveExist(int32 SlotIndex) const;

    UFUNCTION(BlueprintPure, Category = "Save")
    FString GetSaveInfo(int32 SlotIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    TArray<FString> GetAllSaves() const;

protected:
    UPROPERTY()
    FTimerHandle AutoSaveTimer;

    UFUNCTION()
    FWorldSaveData GatherWorldData();

    UFUNCTION()
    void ApplyWorldData(const FWorldSaveData& Data);

    UFUNCTION()
    FString GetSlotName(int32 Index) const;

    virtual void BeginPlay() override;
};
