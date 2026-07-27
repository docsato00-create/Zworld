// DayNightManager.cpp

#include "DayNightManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

ADayNightManager::ADayNightManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bAlwaysRelevant = true;
}

void ADayNightManager::BeginPlay()
{
    Super::BeginPlay();
    if (!ZombieSpeedCurve)
    {
        // Domyślna krzywa: w dzień 0.7, w nocy 1.5
        // Utwórz w BP lub załaduj tutaj
    }
}

void ADayNightManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetLocalRole() == ROLE_Authority)
    {
        float DayProgressPerSecond = 24.0f / (RealMinutesPerGameDay * 60.0f);
        CurrentTimeOfDay += DayProgressPerSecond * DeltaTime;

        if (CurrentTimeOfDay >= 24.0f)
        {
            CurrentTimeOfDay -= 24.0f;
            CurrentDay++;
        }

        NormalizedTime = CurrentTimeOfDay / 24.0f;
        UpdateSunRotation();
        CheckDayNightTransition();
    }
}

void ADayNightManager::UpdateSunRotation()
{
    if (SunLight)
    {
        float Pitch = UKismetMathLibrary::NormalizeAxis((NormalizedTime * 360.0f) - 90.0f);
        FRotator NewRotation(Pitch, 0.0f, 0.0f);
        SunLight->SetActorRotation(NewRotation);
    }
}

void ADayNightManager::CheckDayNightTransition()
{
    bool bShouldBeNight = (CurrentTimeOfDay >= NightStartHour || CurrentTimeOfDay < DayStartHour);

    if (bShouldBeNight != bIsNight)
    {
        bIsNight = bShouldBeNight;
        if (bIsNight)
        {
            float NightDuration = (24.0f - NightStartHour + DayStartHour) / 24.0f * RealMinutesPerGameDay;
            OnNightStarted.Broadcast(NightDuration);
        }
        else
        {
            OnDayStarted.Broadcast();
        }
    }

    OnTimeChanged.Broadcast(NormalizedTime);
}

float ADayNightManager::GetZombieSpeedMultiplier() const
{
    if (ZombieSpeedCurve)
    {
        return ZombieSpeedCurve->GetFloatValue(NormalizedTime);
    }
    return bIsNight ? 1.5f : 0.8f;
}

FString ADayNightManager::GetFormattedTime() const
{
    int32 Hours = FMath::FloorToInt(CurrentTimeOfDay);
    int32 Minutes = FMath::FloorToInt((CurrentTimeOfDay - Hours) * 60.0f);
    return FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
}

void ADayNightManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADayNightManager, CurrentTimeOfDay);
    DOREPLIFETIME(ADayNightManager, NormalizedTime);
    DOREPLIFETIME(ADayNightManager, bIsNight);
    DOREPLIFETIME(ADayNightManager, CurrentDay);
}
