// AntiCheatManager.cpp

#include "AntiCheatManager.h"
#include "SurvivalCharacter.h"
#include "SurvivalComponent.h"
#include "InventoryComponent.h"
#include "WeaponBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

AAntiCheatManager::AAntiCheatManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AAntiCheatManager::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority && bEnableAntiCheat)
    {
        GetWorldTimerManager().SetTimer(CheckTimer, this, &AAntiCheatManager::PerformChecks, CheckInterval, true);
    }
}

void AAntiCheatManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAntiCheatManager::PerformChecks()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC || !PC->PlayerState) continue;

        ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(PC->GetPawn());
        if (!Character) continue;

        // Movement check
        FVector CurrentLoc = Character->GetActorLocation();
        if (!ValidateMovement(Character, CurrentLoc))
        {
            ReportInfraction(PC->PlayerState, ECheatType::SpeedHack, TEXT("Movement speed exceeded limits"));
        }

        // Health check
        USurvivalComponent* SurvComp = Character->FindComponentByClass<USurvivalComponent>();
        if (SurvComp && !ValidateHealth(SurvComp->Stats.CurrentHealth))
        {
            ReportInfraction(PC->PlayerState, ECheatType::InfiniteHealth, TEXT("Health exceeded maximum"));
        }

        TakeSnapshot(PC->PlayerState);
    }
}

bool AAntiCheatManager::ValidateMovement(ACharacter* Character, FVector NewLocation)
{
    if (!Character) return true;

    APlayerState* PS = Character->GetPlayerState();
    if (!PS || !PlayerHistory.Contains(PS)) return true;

    const TArray<FPlayerSnapshot>& History = PlayerHistory[PS];
    if (History.Num() == 0) return true;

    const FPlayerSnapshot& LastSnapshot = History.Last();
    float DeltaTime = GetWorld()->GetTimeSeconds() - LastSnapshot.Timestamp;
    if (DeltaTime <= 0.0f) return true;

    float Distance = FVector::Dist(NewLocation, LastSnapshot.Location);
    float Speed = Distance / DeltaTime;

    // Check horizontal speed
    FVector HorizontalVelocity = FVector(NewLocation.X - LastSnapshot.Location.X, NewLocation.Y - LastSnapshot.Location.Y, 0);
    float HorizontalSpeed = HorizontalVelocity.Size() / DeltaTime;

    // Check vertical speed (fly hack)
    float VerticalSpeed = FMath::Abs(NewLocation.Z - LastSnapshot.Location.Z) / DeltaTime;

    // Allow for falling (gravity)
    if (Character->GetCharacterMovement()->IsFalling() && VerticalSpeed < 2000.0f)
    {
        return true;
    }

    if (HorizontalSpeed > MaxAllowedSpeed)
    {
        LogCheat(PS->GetPlayerName(), ECheatType::SpeedHack,
            FString::Printf(TEXT("Speed: %.0f (max: %.0f)"), HorizontalSpeed, MaxAllowedSpeed));
        return false;
    }

    if (!Character->GetCharacterMovement()->IsFalling() && VerticalSpeed > MaxAllowedVerticalSpeed)
    {
        LogCheat(PS->GetPlayerName(), ECheatType::FlyHack,
            FString::Printf(TEXT("Vertical: %.0f (max: %.0f)"), VerticalSpeed, MaxAllowedVerticalSpeed));
        return false;
    }

    return true;
}

bool AAntiCheatManager::ValidateHealth(float CurrentHealth)
{
    return CurrentHealth <= MaxHealthCap;
}

bool AAntiCheatManager::ValidateDamage(float Damage, AActor* Attacker)
{
    return Damage <= MaxDamagePerShot;
}

bool AAntiCheatManager::ValidateFireRate(float LastFireTime, float CurrentTime)
{
    float Interval = CurrentTime - LastFireTime;
    return Interval >= MinFireInterval || Interval < 0.0f; // Interval < 0 means first shot
}

bool AAntiCheatManager::ValidateItemAdd(APlayerState* Player, TSoftObjectPtr<UItemData> Item, int32 Quantity)
{
    // Check if item is legitimately obtainable
    // Check quantity limits
    // Check if player is near a loot source
    return true; // Placeholder - implement based on game rules
}

void AAntiCheatManager::ReportInfraction(APlayerState* Player, ECheatType CheatType, const FString& Details)
{
    if (!Player) return;

    int32& Count = PlayerInfractions.FindOrAdd(Player);
    Count++;

    LogCheat(Player->GetPlayerName(), CheatType,
        FString::Printf(TEXT("%s | Infraction %d/%d"), *Details, Count, MaxInfractionsBeforeKick));

    if (Count >= MaxInfractionsBeforeKick && bAutoKickEnabled)
    {
        KickPlayer(Player, FString::Printf(TEXT("Anti-cheat: %s"), *UEnum::GetValueAsString(CheatType)));
    }
}

void AAntiCheatManager::KickPlayer(APlayerState* Player, const FString& Reason)
{
    if (!Player) return;

    APlayerController* PC = Cast<APlayerController>(Player->GetOwner());
    if (PC)
    {
        FText KickReason = FText::FromString(Reason);
        GameSession->KickPlayer(PC, KickReason);
    }
}

void AAntiCheatManager::BanPlayer(APlayerState* Player, const FString& Reason, int32 DurationMinutes)
{
    if (!Player) return;

    FString PlayerID = Player->GetUniqueId().ToString();
    float UnbanTime = GetWorld()->GetTimeSeconds() + (DurationMinutes * 60.0f);
    BannedPlayers.Add(PlayerID, UnbanTime);

    KickPlayer(Player, FString::Printf(TEXT("Banned: %s (%d min)"), *Reason, DurationMinutes));

    // Save ban list
    FString BanList;
    for (const auto& Pair : BannedPlayers)
    {
        BanList += FString::Printf(TEXT("%s|%f\n"), *Pair.Key, Pair.Value);
    }
    FFileHelper::SaveStringToFile(BanList, *(FPaths::ProjectSavedDir() / TEXT("BanList.txt")));
}

bool AAntiCheatManager::IsPlayerBanned(const FString& PlayerID) const
{
    if (!BannedPlayers.Contains(PlayerID)) return false;

    float UnbanTime = BannedPlayers[PlayerID];
    return GetWorld()->GetTimeSeconds() < UnbanTime;
}

void AAntiCheatManager::LogCheat(const FString& PlayerName, ECheatType Type, const FString& Details)
{
    FString LogEntry = FString::Printf(TEXT("[%s] %s | %s | %s\n"),
        *FDateTime::Now().ToString(),
        *PlayerName,
        *UEnum::GetValueAsString(Type),
        *Details);

    UE_LOG(LogTemp, Warning, TEXT("%s"), *LogEntry);

    if (bLogToFile)
    {
        FString LogPath = FPaths::ProjectSavedDir() / TEXT("AntiCheat.log");
        FFileHelper::SaveStringToFile(LogEntry, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    }
}

void AAntiCheatManager::TakeSnapshot(APlayerState* Player)
{
    if (!Player) return;

    ACharacter* Character = Cast<ACharacter>(Player->GetPawn());
    if (!Character) return;

    FPlayerSnapshot Snapshot;
    Snapshot.Location = Character->GetActorLocation();
    Snapshot.Velocity = Character->GetVelocity();
    Snapshot.Timestamp = GetWorld()->GetTimeSeconds();

    USurvivalComponent* SurvComp = Character->FindComponentByClass<USurvivalComponent>();
    if (SurvComp)
    {
        Snapshot.Health = SurvComp->Stats.CurrentHealth;
        Snapshot.Stamina = SurvComp->Stats.CurrentStamina;
    }

    TArray<FPlayerSnapshot>& History = PlayerHistory.FindOrAdd(Player);
    History.Add(Snapshot);

    // Keep only last 10 snapshots
    if (History.Num() > 10)
    {
        History.RemoveAt(0);
    }
}
