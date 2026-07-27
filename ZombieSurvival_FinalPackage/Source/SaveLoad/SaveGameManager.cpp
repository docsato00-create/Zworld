// SaveGameManager.cpp

#include "SaveGameManager.h"
#include "SurvivalGameMode.h"
#include "SurvivalPlayerState.h"
#include "SurvivalCharacter.h"
#include "BuildingPiece.h"
#include "ClaimBlock.h"
#include "AssembledVehicle.h"
#include "InventoryComponent.h"
#include "SurvivalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ASaveGameManager::ASaveGameManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void ASaveGameManager::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        GetWorldTimerManager().SetTimer(AutoSaveTimer, this, &ASaveGameManager::AutoSave, AutoSaveInterval, true);
    }
}

void ASaveGameManager::SaveWorld(int32 SlotIndex)
{
    if (GetLocalRole() != ROLE_Authority) return;

    USurvivalSaveGame* SaveGameInstance = Cast<USurvivalSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivalSaveGame::StaticClass()));
    if (!SaveGameInstance) return;

    SaveGameInstance->WorldData = GatherWorldData();

    FString SlotName = GetSlotName(SlotIndex);
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);

    UE_LOG(LogTemp, Log, TEXT("World saved to slot: %s"), *SlotName);
}

void ASaveGameManager::LoadWorld(int32 SlotIndex)
{
    if (GetLocalRole() != ROLE_Authority) return;

    FString SlotName = GetSlotName(SlotIndex);
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0)) return;

    USurvivalSaveGame* SaveGameInstance = Cast<USurvivalSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!SaveGameInstance) return;

    ApplyWorldData(SaveGameInstance->WorldData);
    UE_LOG(LogTemp, Log, TEXT("World loaded from slot: %s"), *SlotName);
}

void ASaveGameManager::AutoSave()
{
    SaveWorld(0); // Auto-save to slot 0
}

FWorldSaveData ASaveGameManager::GatherWorldData()
{
    FWorldSaveData Data;
    Data.Version = CurrentVersion;
    Data.SaveDate = FDateTime::Now().ToString();

    // World state
    if (ASurvivalGameMode* GM = Cast<ASurvivalGameMode>(GetWorld()->GetAuthGameMode()))
    {
        // Data.CurrentDay = GM->GetCurrentDay();
        // Data.TimeOfDay = GM->GetTimeOfDay();
    }

    // Buildings
    TArray<AActor*> Buildings;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildingPiece::StaticClass(), Buildings);
    for (AActor* Actor : Buildings)
    {
        ABuildingPiece* Building = Cast<ABuildingPiece>(Actor);
        if (Building)
        {
            FBuildingSaveData BData;
            BData.BuildingClassPath = Building->GetClass()->GetPathName();
            BData.Location = Building->GetActorLocation();
            BData.Rotation = Building->GetActorRotation();
            BData.Health = Building->CurrentHealth;
            BData.UpgradeLevel = 1; // TODO
            // BData.OwnerPlayerID = Building->OwnerPlayerID;
            Data.Buildings.Add(BData);
        }
    }

    // Vehicles
    TArray<AActor*> Vehicles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAssembledVehicle::StaticClass(), Vehicles);
    for (AActor* Actor : Vehicles)
    {
        AAssembledVehicle* Vehicle = Cast<AAssembledVehicle>(Actor);
        if (Vehicle)
        {
            FVehicleSaveData VData;
            VData.VehicleClassPath = Vehicle->GetClass()->GetPathName();
            VData.Location = Vehicle->GetActorLocation();
            VData.Rotation = Vehicle->GetActorRotation();
            VData.Fuel = Vehicle->CurrentFuel;
            VData.Health = Vehicle->CurrentHealth;
            Data.Vehicles.Add(VData);
        }
    }

    // Players
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        ASurvivalPlayerState* PS = Cast<ASurvivalPlayerState>(PC->PlayerState);
        ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(PC->GetPawn());

        if (PS && Character)
        {
            FPlayerSaveData PData;
            PData.PlayerName = PS->GetPlayerName();
            PData.Level = PS->PlayerLevel;
            PData.XP = PS->CurrentXP;
            PData.SkillPoints = PS->AvailableSkillPoints;
            PData.LastLocation = Character->GetActorLocation();
            PData.ZombiesKilled = PS->ZombiesKilled;
            PData.PlayersKilled = PS->PlayersKilled;
            PData.TimePlayed = PS->TimePlayed;

            UInventoryComponent* InvComp = Character->FindComponentByClass<UInventoryComponent>();
            if (InvComp)
            {
                PData.Inventory = InvComp->Slots;
            }

            USurvivalComponent* SurvComp = Character->FindComponentByClass<USurvivalComponent>();
            if (SurvComp)
            {
                PData.Stats = SurvComp->Stats;
            }

            Data.Players.Add(PData);
        }
    }

    return Data;
}

void ASaveGameManager::ApplyWorldData(const FWorldSaveData& Data)
{
    // Clear existing
    // Spawn buildings
    for (const FBuildingSaveData& BData : Data.Buildings)
    {
        UClass* BuildingClass = LoadObject<UClass>(nullptr, *BData.BuildingClassPath);
        if (BuildingClass)
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            ABuildingPiece* Building = GetWorld()->SpawnActor<ABuildingPiece>(BuildingClass, BData.Location, BData.Rotation, Params);
            if (Building)
            {
                Building->CurrentHealth = BData.Health;
            }
        }
    }

    // Spawn vehicles
    for (const FVehicleSaveData& VData : Data.Vehicles)
    {
        UClass* VehicleClass = LoadObject<UClass>(nullptr, *VData.VehicleClassPath);
        if (VehicleClass)
        {
            FActorSpawnParameters Params;
            AAssembledVehicle* Vehicle = GetWorld()->SpawnActor<AAssembledVehicle>(VehicleClass, VData.Location, VData.Rotation, Params);
            if (Vehicle)
            {
                Vehicle->CurrentFuel = VData.Fuel;
                Vehicle->CurrentHealth = VData.Health;
            }
        }
    }
}

void ASaveGameManager::SavePlayerData(APlayerState* PlayerState)
{
    // Individual player save (for Steam Cloud)
}

void ASaveGameManager::LoadPlayerData(APlayerState* PlayerState)
{
    // Individual player load
}

void ASaveGameManager::DeleteSave(int32 SlotIndex)
{
    FString SlotName = GetSlotName(SlotIndex);
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UGameplayStatics::DeleteGameInSlot(SlotName, 0);
    }
}

bool ASaveGameManager::DoesSaveExist(int32 SlotIndex) const
{
    return UGameplayStatics::DoesSaveGameExist(GetSlotName(SlotIndex), 0);
}

FString ASaveGameManager::GetSaveInfo(int32 SlotIndex) const
{
    FString SlotName = GetSlotName(SlotIndex);
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0)) return TEXT("Empty");

    USurvivalSaveGame* SaveGame = Cast<USurvivalSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (SaveGame)
    {
        return FString::Printf(TEXT("Day %d - %s"), SaveGame->WorldData.CurrentDay, *SaveGame->WorldData.SaveDate);
    }
    return TEXT("Corrupted");
}

TArray<FString> ASaveGameManager::GetAllSaves() const
{
    TArray<FString> Saves;
    for (int32 i = 0; i < MaxSaveSlots; i++)
    {
        Saves.Add(GetSaveInfo(i));
    }
    return Saves;
}

FString ASaveGameManager::GetSlotName(int32 Index) const
{
    return FString::Printf(TEXT("%s_%d"), *SaveSlotPrefix, Index);
}
