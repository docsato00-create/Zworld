// HordeManager.cpp

#include "HordeManager.h"
#include "ZombieBase.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

AHordeManager::AHordeManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AHordeManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetLocalRole() != ROLE_Authority) return;

    // Automatyczne spawnowanie hord w nocy
    // Sprawdź przez DayNightManager
    // if (Night && TimeSinceLastHorde > Interval) SpawnHorde();

    // Aktualizuj pozycje hord
    for (FHordeGroup& Horde : ActiveHordes)
    {
        if (!Horde.bIsActive) continue;

        for (TWeakObjectPtr<AZombieBase> Zombie : Horde.Members)
        {
            if (Zombie.IsValid() && !Zombie->bIsDead)
            {
                Zombie->HordeTargetLocation = Horde.TargetLocation;
                Zombie->bIsInHorde = true;
            }
        }
    }
}

void AHordeManager::TriggerHorde(FVector TargetLocation, int32 Size)
{
    if (GetLocalRole() != ROLE_Authority) return;

    FHordeGroup NewHorde;
    NewHorde.TargetLocation = TargetLocation;
    NewHorde.SpawnTime = GetWorld()->GetTimeSeconds();
    NewHorde.bIsActive = true;

    FVector SpawnLocation = FindHordeSpawnLocation(TargetLocation);

    for (int32 i = 0; i < Size; i++)
    {
        if (!ZombieClass) continue;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        FVector Offset = FVector(FMath::RandRange(-500, 500), FMath::RandRange(-500, 500), 0);
        AZombieBase* NewZombie = GetWorld()->SpawnActor<AZombieBase>(ZombieClass, SpawnLocation + Offset, FRotator::ZeroRotator, Params);

        if (NewZombie)
        {
            NewHorde.Members.Add(NewZombie);
        }
    }

    ActiveHordes.Add(NewHorde);
    LastHordeTime = GetWorld()->GetTimeSeconds();
}

FVector AHordeManager::FindHordeSpawnLocation(FVector Target) const
{
    // Znajdź punkt na krawędzi mapy w kierunku od Target
    FVector Direction = Target.GetSafeNormal();
    FVector SpawnPoint = Target - (Direction * FMath::RandRange(2000.0f, 4000.0f));
    SpawnPoint.Z = Target.Z;
    return SpawnPoint;
}
