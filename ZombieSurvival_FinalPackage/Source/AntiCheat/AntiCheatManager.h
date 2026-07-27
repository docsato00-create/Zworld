// AntiCheatManager.h
// Source/ZombieSurvival/AntiCheat/AntiCheatManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntiCheatManager.generated.h"

USTRUCT()
struct FPlayerSnapshot
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    float Health;

    UPROPERTY()
    float Stamina;

    UPROPERTY()
    int32 Ammo;

    UPROPERTY()
    float Timestamp;
};

UENUM(BlueprintType)
enum class ECheatType : uint8
{
    SpeedHack       UMETA(DisplayName = "Speed Hack"),
    FlyHack         UMETA(DisplayName = "Fly Hack"),
    Teleport        UMETA(DisplayName = "Teleport"),
    InfiniteHealth  UMETA(DisplayName = "Infinite Health"),
    InfiniteAmmo    UMETA(DisplayName = "Infinite Ammo"),
    DamageModifier  UMETA(DisplayName = "Damage Modifier"),
    ItemSpawn       UMETA(DisplayName = "Item Spawn"),
    GodMode         UMETA(DisplayName = "God Mode"),
    NoClip          UMETA(DisplayName = "No Clip"),
    RapidFire       UMETA(DisplayName = "Rapid Fire")
};

UCLASS()
class ZOMBIESURVIVAL_API AAntiCheatManager : public AActor
{
    GENERATED_BODY()

public:
    AAntiCheatManager();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    bool bEnableAntiCheat = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    float MaxAllowedSpeed = 1500.0f; // 1500 uu/s = ~54 km/h

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    float MaxAllowedVerticalSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    float MaxHealthCap = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    float MaxDamagePerShot = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    float MinFireInterval = 0.05f; // 1200 RPM max

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    int32 MaxInfractionsBeforeKick = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    bool bAutoKickEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiCheat")
    bool bLogToFile = true;

    // === VALIDATION ===
    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    bool ValidateMovement(ACharacter* Character, FVector NewLocation);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    bool ValidateHealth(float CurrentHealth);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    bool ValidateDamage(float Damage, AActor* Attacker);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    bool ValidateFireRate(float LastFireTime, float CurrentTime);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    bool ValidateItemAdd(APlayerState* Player, TSoftObjectPtr<class UItemData> Item, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    void ReportInfraction(APlayerState* Player, ECheatType CheatType, const FString& Details);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    void KickPlayer(APlayerState* Player, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "AntiCheat")
    void BanPlayer(APlayerState* Player, const FString& Reason, int32 DurationMinutes);

    UFUNCTION(BlueprintPure, Category = "AntiCheat")
    bool IsPlayerBanned(const FString& PlayerID) const;

protected:
    UPROPERTY()
    TMap<APlayerState*, int32> PlayerInfractions;

    UPROPERTY()
    TMap<APlayerState*, TArray<FPlayerSnapshot>> PlayerHistory;

    UPROPERTY()
    TMap<FString, float> BannedPlayers; // SteamID -> UnbanTime

    UPROPERTY()
    float CheckInterval = 1.0f;

    UPROPERTY()
    FTimerHandle CheckTimer;

    UFUNCTION()
    void PerformChecks();

    UFUNCTION()
    void LogCheat(const FString& PlayerName, ECheatType Type, const FString& Details);

    UFUNCTION()
    void TakeSnapshot(APlayerState* Player);
};
