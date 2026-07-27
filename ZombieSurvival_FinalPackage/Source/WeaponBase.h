// WeaponBase.h
// Source/ZombieSurvival/Weapons/WeaponBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWeaponBase();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // === WEAPON STATS ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FName WeaponName = FName("Pistol");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 0.2f; // seconds between shots

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MagazineSize = 15;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float ReloadTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Spread = 1.0f; // degrees

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsAutomatic = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<class UCameraShakeBase> FireCameraShake;

    // === AMMO ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSoftObjectPtr<class UItemData> AmmoType;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo = 15;

    // === RUNTIME ===
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    bool bIsReloading = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    float CurrentDurability = 100.0f;

    // === FUNCTIONS ===
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire(AActor* Shooter);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StartReload();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void FinishReload();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool CanFire() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool NeedsReload() const { return CurrentAmmo <= 0; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    class USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    class USceneComponent* MuzzleLocation;

    float LastFireTime = 0.0f;

    UFUNCTION(Server, Reliable)
    void Server_Fire(FVector Origin, FVector Direction);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_FireEffects();

    UFUNCTION()
    void ApplyRecoil();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
