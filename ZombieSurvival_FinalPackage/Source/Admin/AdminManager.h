// AdminManager.h
// Source/ZombieSurvival/Admin/AdminManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AdminManager.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AAdminManager : public AActor
{
    GENERATED_BODY()

public:
    AAdminManager();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Admin")
    TArray<FString> AdminSteamIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Admin")
    bool bEnableAdminCommands = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Admin")
    bool bLogAdminActions = true;

    // === ADMIN CHECK ===
    UFUNCTION(BlueprintPure, Category = "Admin")
    bool IsAdmin(APlayerState* Player) const;

    // === COMMANDS ===
    UFUNCTION(BlueprintCallable, Category = "Admin")
    void ProcessCommand(APlayerState* Admin, const FString& Command);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void GiveItem(APlayerState* Target, TSoftObjectPtr<class UItemData> Item, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void TeleportPlayer(APlayerState* Target, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void TeleportToPlayer(APlayerState* Admin, APlayerState* Target);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void KillPlayer(APlayerState* Target);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void HealPlayer(APlayerState* Target);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void SetTimeOfDay(float Hour);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void TriggerHorde(FVector Location, int32 Size);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void ClearZombies();

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void SpawnVehicle(FVector Location, TSubclassOf<class AAssembledVehicle> VehicleClass);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void SetPlayerLevel(APlayerState* Target, int32 Level);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void KickPlayer(APlayerState* Target, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void BanPlayer(APlayerState* Target, const FString& Reason, int32 Minutes);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void UnbanPlayer(const FString& SteamID);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void SaveWorld();

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void LoadWorld();

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void WipeWorld();

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void BroadcastMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void SetServerConfig(const FString& Key, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "Admin")
    void GetServerStats(FString& OutStats);

protected:
    UFUNCTION()
    void LogAdminAction(const FString& AdminName, const FString& Action);

    UFUNCTION()
    void SendAdminMessage(APlayerState* Target, const FString& Message);
};
