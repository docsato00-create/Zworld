// SurvivalGameMode.h
// Source/ZombieSurvival/Multiplayer/SurvivalGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurvivalGameMode.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API ASurvivalGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASurvivalGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    bool bEnablePvP = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    bool bEnableRaiding = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float RaidWindowStartHour = 18.0f; // 18:00

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float RaidWindowEndHour = 6.0f; // 06:00

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float RespawnDelay = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<FVector> SpawnPoints;

    UFUNCTION(BlueprintCallable, Category = "Game Rules")
    bool IsRaidingAllowed() const;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FVector FindSpawnPoint() const;

protected:
    UPROPERTY()
    class ADayNightManager* DayNightManager;

    UPROPERTY()
    class AHordeManager* HordeManager;

    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
