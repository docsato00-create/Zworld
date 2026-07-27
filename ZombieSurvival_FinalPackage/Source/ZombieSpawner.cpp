// ZombieSpawner.cpp

#include "ZombieSpawner.h"
#include "ZombieBase.h"
#include "DayNightManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AZombieSpawner::AZombieSpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AZombieSpawner::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> DayNightManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightManager::StaticClass(), DayNightManagers);
    if (DayNightManagers.Num() > 0)
    {
        DayNightManager = Cast<ADayNightManager>(DayNightManagers[0]);
    }

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AZombieSpawner::TrySpawn, SpawnInterval, true);
}

void AZombieSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Cleanup dead zombies from array
    SpawnedZombies.RemoveAll([](TWeakObjectPtr<AZombieBase> Zombie)
    {
        return !Zombie.IsValid() || Zombie->bIsDead;
    });
}

void AZombieSpawner::TrySpawn()
{
    if (GetLocalRole() != ROLE_Authority) return;

    int32 AliveCount = GetAliveZombieCount();
    if (AliveCount >= MaxZombies) return;

    if (bOnlySpawnAtNight && DayNightManager && !DayNightManager->IsNight()) return;

    float Multiplier = GetCurrentSpawnMultiplier();
    int32 SpawnCount = FMath::RandRange(1, FMath::FloorToInt(3 * Multiplier));
    SpawnCount = FMath::Min(SpawnCount, MaxZombies - AliveCount);

    for (int32 i = 0; i < SpawnCount; i++)
    {
        FVector SpawnLocation = FindSpawnLocation();
        TSubclassOf<AZombieBase> ZombieClass = SelectZombieType();

        if (!ZombieClass) continue;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AZombieBase* NewZombie = GetWorld()->SpawnActor<AZombieBase>(ZombieClass, SpawnLocation, FRotator::ZeroRotator, Params);
        if (NewZombie)
        {
            SpawnedZombies.Add(NewZombie);

            // Apply time of day modifier
            if (DayNightManager)
            {
                float SpeedMult = DayNightManager->GetZombieSpeedMultiplier();
                NewZombie->ApplyTimeOfDayModifier(SpeedMult, SpeedMult);
            }
        }
    }
}

void AZombieSpawner::ClearAllZombies()
{
    for (TWeakObjectPtr<AZombieBase> Zombie : SpawnedZombies)
    {
        if (Zombie.IsValid())
        {
            Zombie->Destroy();
        }
    }
    SpawnedZombies.Empty();
}

int32 AZombieSpawner::GetAliveZombieCount() const
{
    int32 Count = 0;
    for (TWeakObjectPtr<AZombieBase> Zombie : SpawnedZombies)
    {
        if (Zombie.IsValid() && !Zombie->bIsDead)
        {
            Count++;
        }
    }
    return Count;
}

FVector AZombieSpawner::FindSpawnLocation() const
{
    FVector Origin = GetActorLocation();
    FVector RandomOffset = FVector(
        FMath::FRandRange(-SpawnRadius, SpawnRadius),
        FMath::FRandRange(-SpawnRadius, SpawnRadius),
        0.0f
    );

    FVector Candidate = Origin + RandomOffset;

    // Find ground
    FHitResult Hit;
    FVector TraceStart = Candidate + FVector(0, 0, 1000.0f);
    FVector TraceEnd = Candidate - FVector(0, 0, 1000.0f);

    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic))
    {
        return Hit.Location + FVector(0, 0, 50.0f);
    }

    return Candidate;
}

TSubclassOf<AZombieBase> AZombieSpawner::SelectZombieType() const
{
    if (ZombieTypes.Num() == 0) return nullptr;

    // Night = więcej runnerów i tanków
    if (DayNightManager && DayNightManager->IsNight())
    {
        return ZombieTypes[FMath::RandRange(0, FMath::Min(2, ZombieTypes.Num() - 1))];
    }

    return ZombieTypes[0]; // Day = mostly walkers
}

float AZombieSpawner::GetCurrentSpawnMultiplier() const
{
    if (!DayNightManager) return 1.0f;
    return DayNightManager->IsNight() ? NightSpawnMultiplier : DaySpawnMultiplier;
}
