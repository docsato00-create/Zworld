// BuildingPiece.cpp

#include "BuildingPiece.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ABuildingPiece::ABuildingPiece()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    RootComponent = BuildingMesh;

    BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BuildingCollision"));
    BuildingCollision->SetupAttachment(RootComponent);
    BuildingCollision->SetBoxExtent(GridSize / 2.0f);
    BuildingCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BuildingCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    BuildingCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    CurrentHealth = MaxHealth;
}

void ABuildingPiece::TakeBuildingDamage(float Damage, AActor* DamageCauser)
{
    if (bIsDestroyed) return;

    CurrentHealth -= Damage;
    OnRep_Health();

    if (CurrentHealth <= 0.0f)
    {
        DestroyBuilding();
    }
}

void ABuildingPiece::Repair(float Amount)
{
    if (bIsDestroyed) return;
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
    OnRep_Health();
}

void ABuildingPiece::DestroyBuilding()
{
    bIsDestroyed = true;
    CurrentHealth = 0.0f;

    // Drop surowców (50% zwrotu)
    // Spawn debris

    BuildingMesh->SetSimulatePhysics(true);
    BuildingMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    SetLifeSpan(60.0f); // Zniknij po minucie
}

void ABuildingPiece::OnRep_Health()
{
    // Update material / crack texture
    float HealthPercent = GetHealthPercent();
    // Dynamic material
}

void ABuildingPiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABuildingPiece, CurrentHealth);
    DOREPLIFETIME(ABuildingPiece, bIsDestroyed);
}
