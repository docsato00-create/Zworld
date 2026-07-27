// ClaimBlock.cpp

#include "ClaimBlock.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerState.h"

AClaimBlock::AClaimBlock()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    RootComponent = BlockMesh;

    ClaimRadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ClaimRadiusSphere"));
    ClaimRadiusSphere->SetupAttachment(RootComponent);
    ClaimRadiusSphere->SetSphereRadius(ClaimRadius);
    ClaimRadiusSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ClaimRadiusSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    ClaimRadiusSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    CurrentHealth = MaxHealth;
}

void AClaimBlock::BeginPlay()
{
    Super::BeginPlay();
    ClaimRadiusSphere->SetSphereRadius(ClaimRadius);
}

void AClaimBlock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AClaimBlock::TakeDamage(float Damage, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.0f) return;

    CurrentHealth -= Damage;
    OnRep_Health();

    if (CurrentHealth <= 0.0f)
    {
        // Destroy claim block → wszystkie struktury w promieniu tracą ochronę
        Destroy();
    }
}

void AClaimBlock::Upgrade()
{
    if (UpgradeLevel >= MaxUpgradeLevel) return;
    UpgradeLevel++;
    MaxHealth *= 1.5f;
    CurrentHealth = MaxHealth;
    ClaimRadius *= 1.2f;
    ClaimRadiusSphere->SetSphereRadius(ClaimRadius);
}

bool AClaimBlock::CanBuildHere(AActor* Builder) const
{
    if (!Builder) return false;
    APlayerState* BuilderPS = Builder->GetInstigatorController() ? Builder->GetInstigatorController()->PlayerState : nullptr;
    return (BuilderPS == OwnerPlayerState);
}

bool AClaimBlock::IsWithinClaim(FVector Location) const
{
    return FVector::Dist(Location, GetActorLocation()) <= ClaimRadius;
}

void AClaimBlock::ApplyDecay(float DeltaHours)
{
    CurrentHealth -= DecayRatePerHour * DeltaHours;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    OnRep_Health();
}

void AClaimBlock::OnRep_Health()
{
    // Update UI / health bar / material
    float HealthPercent = CurrentHealth / MaxHealth;
    // Dynamic material instance update
}

void AClaimBlock::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Opcjonalnie: buff dla właściciela w obrębie claima
}

void AClaimBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AClaimBlock, CurrentHealth);
    DOREPLIFETIME(AClaimBlock, UpgradeLevel);
    DOREPLIFETIME(AClaimBlock, OwnerPlayerState);
}
