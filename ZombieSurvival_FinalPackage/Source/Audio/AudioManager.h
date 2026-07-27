// AudioManager.h
// Source/ZombieSurvival/Audio/AudioManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioManager.generated.h"

UENUM(BlueprintType)
enum class ESoundType : uint8
{
    Footstep        UMETA(DisplayName = "Footstep"),
    Gunshot         UMETA(DisplayName = "Gunshot"),
    Reload          UMETA(DisplayName = "Reload"),
    MeleeSwing      UMETA(DisplayName = "Melee Swing"),
    MeleeHit        UMETA(DisplayName = "Melee Hit"),
    ZombieGroan     UMETA(DisplayName = "Zombie Groan"),
    ZombieAttack    UMETA(DisplayName = "Zombie Attack"),
    ZombieDeath     UMETA(DisplayName = "Zombie Death"),
    PlayerHit       UMETA(DisplayName = "Player Hit"),
    PlayerDeath     UMETA(DisplayName = "Player Death"),
    BuildingPlace   UMETA(DisplayName = "Building Place"),
    BuildingDestroy UMETA(DisplayName = "Building Destroy"),
    VehicleStart    UMETA(DisplayName = "Vehicle Start"),
    VehicleDrive    UMETA(DisplayName = "Vehicle Drive"),
    VehicleCrash    UMETA(DisplayName = "Vehicle Crash"),
    AmbientDay      UMETA(DisplayName = "Ambient Day"),
    AmbientNight    UMETA(DisplayName = "Ambient Night"),
    HordeAlert      UMETA(DisplayName = "Horde Alert"),
    UIHover         UMETA(DisplayName = "UI Hover"),
    UIClick         UMETA(DisplayName = "UI Click"),
    InventoryOpen   UMETA(DisplayName = "Inventory Open"),
    Eat             UMETA(DisplayName = "Eat"),
    Drink           UMETA(DisplayName = "Drink"),
    Craft           UMETA(DisplayName = "Craft"),
    Rain            UMETA(DisplayName = "Rain"),
    Thunder         UMETA(DisplayName = "Thunder")
};

USTRUCT(BlueprintType)
struct FSoundEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESoundType SoundType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<USoundBase*> Sounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VolumeMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PitchMin = 0.95f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PitchMax = 1.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxDistance = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIs3D = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class USoundAttenuation> Attenuation;
};

UCLASS()
class ZOMBIESURVIVAL_API AAudioManager : public AActor
{
    GENERATED_BODY()

public:
    AAudioManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TArray<FSoundEntry> SoundLibrary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* DayAmbientMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* NightAmbientMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* CombatMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MusicFadeTime = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MasterVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float SFXVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MusicVolume = 0.7f;

    // === FUNCTIONS ===
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySound(ESoundType Type, FVector Location, float VolumeScale = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySound2D(ESoundType Type, float VolumeScale = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayFootstep(ETerrainType Terrain, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StartMusic(USoundBase* Music);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopMusic(float FadeOutTime = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void UpdateAmbientMusic(bool bIsNight);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayHordeAlert();

protected:
    UPROPERTY()
    class UAudioComponent* MusicComponent;

    UPROPERTY()
    class UAudioComponent* AmbientComponent;

    UPROPERTY()
    TArray<UAudioComponent*> ActiveSFXComponents;

    UPROPERTY()
    class ADayNightManager* DayNightManager;

    UFUNCTION()
    FSoundEntry* FindSoundEntry(ESoundType Type);

    UFUNCTION()
    void CleanupFinishedSounds();
};
