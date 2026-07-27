// DayNightManager.h
// Source/ZombieSurvival/Environment/DayNightManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, float, NormalizedTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNightStarted, float, NightDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayStarted);

UCLASS()
class ZOMBIESURVIVAL_API ADayNightManager : public AActor
{
    GENERATED_BODY()

public:
    ADayNightManager();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // === CONFIG ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    float RealMinutesPerGameDay = 45.0f; // 45 min real = 24h in-game

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    float DayStartHour = 6.0f;  // 6:00

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    float NightStartHour = 20.0f; // 20:00

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    UCurveFloat* ZombieSpeedCurve; // X: time of day (0-1), Y: speed multiplier

    // === RUNTIME ===
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "DayNight")
    float CurrentTimeOfDay = 6.0f; // 0-24

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "DayNight")
    float NormalizedTime = 0.25f; // 0-1

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "DayNight")
    bool bIsNight = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "DayNight")
    int32 CurrentDay = 1;

    // === GETTERS ===
    UFUNCTION(BlueprintPure, Category = "DayNight")
    float GetZombieSpeedMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "DayNight")
    bool IsNight() const { return bIsNight; }

    UFUNCTION(BlueprintPure, Category = "DayNight")
    FString GetFormattedTime() const;

    // === DELEGATES ===
    UPROPERTY(BlueprintAssignable, Category = "DayNight")
    FOnTimeOfDayChanged OnTimeChanged;

    UPROPERTY(BlueprintAssignable, Category = "DayNight")
    FOnNightStarted OnNightStarted;

    UPROPERTY(BlueprintAssignable, Category = "DayNight")
    FOnDayStarted OnDayStarted;

protected:
    UPROPERTY(EditAnywhere, Category = "DayNight")
    ADirectionalLight* SunLight;

    UPROPERTY(EditAnywhere, Category = "DayNight")
    USkyAtmosphereComponent* SkyAtmosphere;

    void UpdateSunRotation();
    void CheckDayNightTransition();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
