// ZombieBase.cpp

#include "ZombieBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DayNightManager.h"

AZombieBase::AZombieBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    CurrentHealth = BaseHealth;
}

void AZombieBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = BaseHealth;
}

void AZombieBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Synchronizacja prędkości z movement component
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * CurrentSpeedMultiplier;
    }
}

void AZombieBase::ApplyTimeOfDayModifier(float SpeedMultiplier, float DamageMultiplier)
{
    CurrentSpeedMultiplier = SpeedMultiplier;
    CurrentDamageMultiplier = DamageMultiplier;
}

void AZombieBase::TakeZombieDamage(float Damage, AActor* DamageDealer)
{
    if (bIsDead) return;

    CurrentHealth -= Damage;
    OnRep_Health();

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AZombieBase::AttackTarget(AActor* Target)
{
    if (bIsDead || !Target) return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime < AttackCooldown) return;

    LastAttackTime = CurrentTime;

    float FinalDamage = BaseDamage * CurrentDamageMultiplier;

    // Zastosuj damage do gracza
    // UGameplayStatics::ApplyDamage(Target, FinalDamage, GetController(), this, UDamageType::StaticClass());

    // Animacja ataku
    // PlayAnimMontage(AttackMontage);
}

void AZombieBase::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    // Wyłącz collision
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    // Drop loot
    // SpawnLoot();

    // Usuń po 5 minutach (lub zrzuć do object pool)
    SetLifeSpan(300.0f);
}

void AZombieBase::OnRep_Health()
{
    // Update UI / health bar
}

void AZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AZombieBase, CurrentHealth);
    DOREPLIFETIME(AZombieBase, bIsDead);
}
