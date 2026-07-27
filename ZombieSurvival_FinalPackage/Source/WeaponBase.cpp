// WeaponBase.cpp

#include "WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "SurvivalCharacter.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(WeaponMesh, FName("MuzzleSocket"));

    CurrentAmmo = MagazineSize;
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeaponBase::Fire(AActor* Shooter)
{
    if (!CanFire() || !Shooter) return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastFireTime < FireRate) return;

    LastFireTime = CurrentTime;
    CurrentAmmo--;
    CurrentDurability -= 0.5f;

    FVector Origin = MuzzleLocation->GetComponentLocation();
    FVector Direction = MuzzleLocation->GetForwardVector();

    // Spread
    float SpreadRad = FMath::DegreesToRadians(Spread);
    Direction = FMath::VRandCone(Direction, SpreadRad);

    Server_Fire(Origin, Direction);
    Multicast_FireEffects();
    ApplyRecoil();
}

void AWeaponBase::Server_Fire_Implementation(FVector Origin, FVector Direction)
{
    FHitResult Hit;
    FVector End = Origin + (Direction * Range);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Pawn, Params))
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            UGameplayStatics::ApplyDamage(HitActor, Damage, GetInstigatorController(), this, nullptr);
        }
    }
}

void AWeaponBase::Multicast_FireEffects_Implementation()
{
    // Muzzle flash, sound, tracer
    if (FireCameraShake)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            PC->ClientStartCameraShake(FireCameraShake);
        }
    }
}

void AWeaponBase::ApplyRecoil()
{
    // Apply recoil to player controller
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->AddPitchInput(-0.5f);
        PC->AddYawInput(FMath::FRandRange(-0.2f, 0.2f));
    }
}

void AWeaponBase::StartReload()
{
    if (bIsReloading || CurrentAmmo >= MagazineSize) return;

    bIsReloading = true;
    FTimerHandle ReloadTimer;
    GetWorldTimerManager().SetTimer(ReloadTimer, this, &AWeaponBase::FinishReload, ReloadTime, false);
}

void AWeaponBase::FinishReload()
{
    // TODO: Check inventory for ammo
    CurrentAmmo = MagazineSize;
    bIsReloading = false;
}

bool AWeaponBase::CanFire() const
{
    return !bIsReloading && CurrentAmmo > 0 && CurrentDurability > 0.0f;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWeaponBase, CurrentAmmo);
    DOREPLIFETIME(AWeaponBase, bIsReloading);
    DOREPLIFETIME(AWeaponBase, CurrentDurability);
}
