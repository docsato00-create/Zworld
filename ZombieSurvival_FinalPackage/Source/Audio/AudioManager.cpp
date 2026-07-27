// AudioManager.cpp

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
#include "DayNightManager.h"
#include "Kismet/GameplayStatics.h"

AAudioManager::AAudioManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = false; // Audio is client-side

    MusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
    RootComponent = MusicComponent;

    AmbientComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientComponent"));
    AmbientComponent->SetupAttachment(RootComponent);
}

void AAudioManager::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> DayNightManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightManager::StaticClass(), DayNightManagers);
    if (DayNightManagers.Num() > 0)
    {
        DayNightManager = Cast<ADayNightManager>(DayNightManagers[0]);
        if (DayNightManager)
        {
            DayNightManager->OnNightStarted.AddDynamic(this, &AAudioManager::UpdateAmbientMusic);
            DayNightManager->OnDayStarted.AddDynamic(this, &AAudioManager::UpdateAmbientMusic);
        }
    }

    // Start day ambient
    if (DayAmbientMusic)
    {
        AmbientComponent->SetSound(DayAmbientMusic);
        AmbientComponent->Play();
    }
}

void AAudioManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CleanupFinishedSounds();
}

void AAudioManager::PlaySound(ESoundType Type, FVector Location, float VolumeScale)
{
    FSoundEntry* Entry = FindSoundEntry(Type);
    if (!Entry || Entry->Sounds.Num() == 0) return;

    USoundBase* Sound = Entry->Sounds[FMath::RandRange(0, Entry->Sounds.Num() - 1)];
    if (!Sound) return;

    float Pitch = FMath::FRandRange(Entry->PitchMin, Entry->PitchMax);
    float Volume = Entry->VolumeMultiplier * SFXVolume * MasterVolume * VolumeScale;

    if (Entry->bIs3D)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Volume, Pitch, 0.0f, Entry->Attenuation);
    }
    else
    {
        UGameplayStatics::PlaySound2D(GetWorld(), Sound, Volume, Pitch);
    }
}

void AAudioManager::PlaySound2D(ESoundType Type, float VolumeScale)
{
    PlaySound(Type, FVector::ZeroVector, VolumeScale);
}

void AAudioManager::PlayFootstep(ETerrainType Terrain, FVector Location)
{
    // Map terrain to footstep sound type
    PlaySound(ESoundType::Footstep, Location);
}

void AAudioManager::StartMusic(USoundBase* Music)
{
    if (!Music) return;

    if (MusicComponent->IsPlaying())
    {
        MusicComponent->FadeOut(MusicFadeTime, 0.0f);
    }

    MusicComponent->SetSound(Music);
    MusicComponent->FadeIn(MusicFadeTime, MusicVolume * MasterVolume);
}

void AAudioManager::StopMusic(float FadeOutTime)
{
    if (MusicComponent->IsPlaying())
    {
        MusicComponent->FadeOut(FadeOutTime, 0.0f);
    }
}

void AAudioManager::SetMasterVolume(float Volume)
{
    MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    MusicComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
    AmbientComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
}

void AAudioManager::SetSFXVolume(float Volume)
{
    SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void AAudioManager::SetMusicVolume(float Volume)
{
    MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    MusicComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
    AmbientComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
}

void AAudioManager::UpdateAmbientMusic(bool bIsNight)
{
    USoundBase* NewAmbient = bIsNight ? NightAmbientMusic : DayAmbientMusic;
    if (NewAmbient && AmbientComponent->Sound != NewAmbient)
    {
        AmbientComponent->FadeOut(MusicFadeTime, 0.0f);
        AmbientComponent->SetSound(NewAmbient);
        AmbientComponent->FadeIn(MusicFadeTime, MusicVolume * MasterVolume);
    }
}

void AAudioManager::PlayHordeAlert()
{
    PlaySound2D(ESoundType::HordeAlert, 1.5f);
}

FSoundEntry* AAudioManager::FindSoundEntry(ESoundType Type)
{
    for (FSoundEntry& Entry : SoundLibrary)
    {
        if (Entry.SoundType == Type)
        {
            return &Entry;
        }
    }
    return nullptr;
}

void AAudioManager::CleanupFinishedSounds()
{
    ActiveSFXComponents.RemoveAll([](UAudioComponent* Comp)
    {
        return !Comp || !Comp->IsPlaying();
    });
}
