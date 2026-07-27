// SurvivalGameMode.cpp

#include "SurvivalGameMode.h"
#include "SurvivalCharacter.h"
#include "SurvivalPlayerController.h"
#include "SurvivalPlayerState.h"
#include "DayNightManager.h"
#include "HordeManager.h"
#include "Kismet/GameplayStatics.h"

ASurvivalGameMode::ASurvivalGameMode()
{
    DefaultPawnClass = ASurvivalCharacter::StaticClass();
    PlayerControllerClass = ASurvivalPlayerController::StaticClass();
    PlayerStateClass = ASurvivalPlayerState::StaticClass();
}

void ASurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // Spawn DayNightManager i HordeManager
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    DayNightManager = GetWorld()->SpawnActor<ADayNightManager>(ADayNightManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
    HordeManager = GetWorld()->SpawnActor<AHordeManager>(AHordeManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
}

void ASurvivalGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    // Logika post-login: przywitanie, sprawdzenie bana, itp.
}

void ASurvivalGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    // Zapisz stan gracza, timestamp logoutu (dla decay)
}

bool ASurvivalGameMode::IsRaidingAllowed() const
{
    if (!bEnableRaiding || !DayNightManager) return false;

    float CurrentHour = DayNightManager->CurrentTimeOfDay;
    if (RaidWindowStartHour > RaidWindowEndHour)
    {
        // Nocny window (np. 18:00 - 06:00)
        return (CurrentHour >= RaidWindowStartHour || CurrentHour < RaidWindowEndHour);
    }
    else
    {
        return (CurrentHour >= RaidWindowStartHour && CurrentHour < RaidWindowEndHour);
    }
}

FVector ASurvivalGameMode::FindSpawnPoint() const
{
    if (SpawnPoints.Num() > 0)
    {
        return SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
    }
    return FVector::ZeroVector;
}

void ASurvivalGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    // Spawn gracza w losowym punkcie
    if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(NewPlayer->GetPawn()))
    {
        Character->SetActorLocation(FindSpawnPoint());
    }
}
