// AdminManager.cpp

#include "AdminManager.h"
#include "SurvivalCharacter.h"
#include "SurvivalComponent.h"
#include "InventoryComponent.h"
#include "SurvivalPlayerState.h"
#include "DayNightManager.h"
#include "HordeManager.h"
#include "SaveGameManager.h"
#include "AntiCheatManager.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AAdminManager::AAdminManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AAdminManager::BeginPlay()
{
    Super::BeginPlay();
}

bool AAdminManager::IsAdmin(APlayerState* Player) const
{
    if (!Player) return false;
    if (!bEnableAdminCommands) return false;

    FString SteamID = Player->GetUniqueId().ToString();
    return AdminSteamIDs.Contains(SteamID);
}

void AAdminManager::ProcessCommand(APlayerState* Admin, const FString& Command)
{
    if (!IsAdmin(Admin)) return;

    TArray<FString> Tokens;
    Command.ParseIntoArray(Tokens, TEXT(" "), true);
    if (Tokens.Num() == 0) return;

    FString Cmd = Tokens[0].ToLower();

    if (Cmd == TEXT("/give") && Tokens.Num() >= 3)
    {
        // /give PlayerName ItemName Quantity
    }
    else if (Cmd == TEXT("/tp") && Tokens.Num() >= 4)
    {
        // /tp PlayerName X Y Z
    }
    else if (Cmd == TEXT("/kill") && Tokens.Num() >= 2)
    {
        // /kill PlayerName
    }
    else if (Cmd == TEXT("/heal"))
    {
        HealPlayer(Admin);
    }
    else if (Cmd == TEXT("/time") && Tokens.Num() >= 2)
    {
        SetTimeOfDay(FCString::Atof(*Tokens[1]));
    }
    else if (Cmd == TEXT("/horde") && Tokens.Num() >= 2)
    {
        TriggerHorde(FVector::ZeroVector, FCString::Atoi(*Tokens[1]));
    }
    else if (Cmd == TEXT("/clearzombies"))
    {
        ClearZombies();
    }
    else if (Cmd == TEXT("/save"))
    {
        SaveWorld();
    }
    else if (Cmd == TEXT("/wipe"))
    {
        WipeWorld();
    }
    else if (Cmd == TEXT("/broadcast") && Tokens.Num() >= 2)
    {
        FString Msg = Command.RightChop(11); // Remove "/broadcast "
        BroadcastMessage(Msg);
    }
    else if (Cmd == TEXT("/kick") && Tokens.Num() >= 2)
    {
        // /kick PlayerName Reason
    }
    else if (Cmd == TEXT("/ban") && Tokens.Num() >= 2)
    {
        // /ban PlayerName Minutes Reason
    }

    LogAdminAction(Admin->GetPlayerName(), Command);
}

void AAdminManager::GiveItem(APlayerState* Target, TSoftObjectPtr<UItemData> Item, int32 Quantity)
{
    if (!Target) return;
    ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Target->GetPawn());
    if (!Character) return;

    UInventoryComponent* InvComp = Character->FindComponentByClass<UInventoryComponent>();
    if (InvComp)
    {
        InvComp->AddItem(Item, Quantity);
    }
}

void AAdminManager::TeleportPlayer(APlayerState* Target, FVector Location)
{
    if (!Target) return;
    ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Target->GetPawn());
    if (Character)
    {
        Character->SetActorLocation(Location);
    }
}

void AAdminManager::TeleportToPlayer(APlayerState* Admin, APlayerState* Target)
{
    if (!Admin || !Target) return;
    ASurvivalCharacter* TargetChar = Cast<ASurvivalCharacter>(Target->GetPawn());
    if (TargetChar)
    {
        TeleportPlayer(Admin, TargetChar->GetActorLocation());
    }
}

void AAdminManager::KillPlayer(APlayerState* Target)
{
    if (!Target) return;
    ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Target->GetPawn());
    if (Character)
    {
        USurvivalComponent* SurvComp = Character->FindComponentByClass<USurvivalComponent>();
        if (SurvComp)
        {
            SurvComp->ModifyHealth(-9999.0f);
        }
    }
}

void AAdminManager::HealPlayer(APlayerState* Target)
{
    if (!Target) return;
    ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Target->GetPawn());
    if (Character)
    {
        USurvivalComponent* SurvComp = Character->FindComponentByClass<USurvivalComponent>();
        if (SurvComp)
        {
            SurvComp->ModifyHealth(9999.0f);
            SurvComp->ModifyHunger(9999.0f);
            SurvComp->ModifyThirst(9999.0f);
            SurvComp->ModifyStamina(9999.0f);
        }
    }
}

void AAdminManager::SetTimeOfDay(float Hour)
{
    TArray<AActor*> DayNightManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightManager::StaticClass(), DayNightManagers);
    if (DayNightManagers.Num() > 0)
    {
        ADayNightManager* DNM = Cast<ADayNightManager>(DayNightManagers[0]);
        if (DNM)
        {
            DNM->CurrentTimeOfDay = FMath::Clamp(Hour, 0.0f, 24.0f);
        }
    }
}

void AAdminManager::TriggerHorde(FVector Location, int32 Size)
{
    TArray<AActor*> HordeManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHordeManager::StaticClass(), HordeManagers);
    if (HordeManagers.Num() > 0)
    {
        AHordeManager* HM = Cast<AHordeManager>(HordeManagers[0]);
        if (HM)
        {
            HM->TriggerHorde(Location, Size);
        }
    }
}

void AAdminManager::ClearZombies()
{
    TArray<AActor*> Zombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieBase::StaticClass(), Zombies);
    for (AActor* Zombie : Zombies)
    {
        Zombie->Destroy();
    }
}

void AAdminManager::SpawnVehicle(FVector Location, TSubclassOf<AAssembledVehicle> VehicleClass)
{
    if (!VehicleClass) return;
    FActorSpawnParameters Params;
    GetWorld()->SpawnActor<AAssembledVehicle>(VehicleClass, Location, FRotator::ZeroRotator, Params);
}

void AAdminManager::SetPlayerLevel(APlayerState* Target, int32 Level)
{
    ASurvivalPlayerState* PS = Cast<ASurvivalPlayerState>(Target);
    if (PS)
    {
        PS->PlayerLevel = Level;
        PS->AvailableSkillPoints = Level - 1;
    }
}

void AAdminManager::KickPlayer(APlayerState* Target, const FString& Reason)
{
    AAntiCheatManager* ACM = nullptr; // Get from world
    if (ACM)
    {
        ACM->KickPlayer(Target, Reason);
    }
}

void AAdminManager::BanPlayer(APlayerState* Target, const FString& Reason, int32 Minutes)
{
    AAntiCheatManager* ACM = nullptr; // Get from world
    if (ACM)
    {
        ACM->BanPlayer(Target, Reason, Minutes);
    }
}

void AAdminManager::UnbanPlayer(const FString& SteamID)
{
    AAntiCheatManager* ACM = nullptr; // Get from world
    if (ACM)
    {
        // ACM->UnbanPlayer(SteamID);
    }
}

void AAdminManager::SaveWorld()
{
    TArray<AActor*> SaveManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASaveGameManager::StaticClass(), SaveManagers);
    if (SaveManagers.Num() > 0)
    {
        ASaveGameManager* SGM = Cast<ASaveGameManager>(SaveManagers[0]);
        if (SGM)
        {
            SGM->SaveWorld(0);
        }
    }
}

void AAdminManager::LoadWorld()
{
    TArray<AActor*> SaveManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASaveGameManager::StaticClass(), SaveManagers);
    if (SaveManagers.Num() > 0)
    {
        ASaveGameManager* SGM = Cast<ASaveGameManager>(SaveManagers[0]);
        if (SGM)
        {
            SGM->LoadWorld(0);
        }
    }
}

void AAdminManager::WipeWorld()
{
    // Destroy all player-built structures, vehicles, etc.
    // Keep map terrain
    ClearZombies();

    TArray<AActor*> Buildings;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildingPiece::StaticClass(), Buildings);
    for (AActor* Building : Buildings)
    {
        Building->Destroy();
    }

    TArray<AActor*> Vehicles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAssembledVehicle::StaticClass(), Vehicles);
    for (AActor* Vehicle : Vehicles)
    {
        Vehicle->Destroy();
    }

    // Reset player inventories and positions
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(PC->GetPawn());
        if (Character)
        {
            USurvivalComponent* SurvComp = Character->FindComponentByClass<USurvivalComponent>();
            if (SurvComp)
            {
                SurvComp->Respawn();
            }

            UInventoryComponent* InvComp = Character->FindComponentByClass<UInventoryComponent>();
            if (InvComp)
            {
                InvComp->Slots.Empty();
            }
        }
    }
}

void AAdminManager::BroadcastMessage(const FString& Message)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC)
        {
            PC->ClientMessage(FString::Printf(TEXT("[SERVER] %s"), *Message));
        }
    }
}

void AAdminManager::SetServerConfig(const FString& Key, const FString& Value)
{
    // Save to config file
}

void AAdminManager::GetServerStats(FString& OutStats)
{
    int32 PlayerCount = GetWorld()->GetNumPlayerControllers();
    int32 ZombieCount = 0;

    TArray<AActor*> Zombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieBase::StaticClass(), Zombies);
    ZombieCount = Zombies.Num();

    OutStats = FString::Printf(TEXT("Players: %d | Zombies: %d | Uptime: %.0f min"),
        PlayerCount, ZombieCount, GetWorld()->GetTimeSeconds() / 60.0f);
}

void AAdminManager::LogAdminAction(const FString& AdminName, const FString& Action)
{
    if (!bLogAdminActions) return;

    FString LogEntry = FString::Printf(TEXT("[%s] ADMIN: %s | %s\n"),
        *FDateTime::Now().ToString(), *AdminName, *Action);

    UE_LOG(LogTemp, Log, TEXT("%s"), *LogEntry);
}

void AAdminManager::SendAdminMessage(APlayerState* Target, const FString& Message)
{
    if (!Target) return;
    APlayerController* PC = Cast<APlayerController>(Target->GetOwner());
    if (PC)
    {
        PC->ClientMessage(Message);
    }
}
