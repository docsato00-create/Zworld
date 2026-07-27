// SurvivalComponent.cpp
// Source/ZombieSurvival/Player/SurvivalComponent.cpp

#include "SurvivalComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USurvivalComponent::USurvivalComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void USurvivalComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (GetOwnerRole() == ROLE_Authority && !bIsDead)
    {
        ApplyPassiveDecay(DeltaTime);
        CheckCriticalLevels();
    }
}

void USurvivalComponent::ApplyPassiveDecay(float DeltaTime)
{
    // Głód i pragnienie spadają co minutę → przeliczamy na DeltaTime
    const float MinuteFraction = DeltaTime / 60.0f;

    Stats.CurrentHunger -= Stats.HungerDecayPerMinute * MinuteFraction;
    Stats.CurrentThirst -= Stats.ThirstDecayPerMinute * MinuteFraction;

    Stats.CurrentHunger = FMath::Clamp(Stats.CurrentHunger, 0.0f, Stats.MaxHunger);
    Stats.CurrentThirst = FMath::Clamp(Stats.CurrentThirst, 0.0f, Stats.MaxThirst);

    // Regeneracja staminy jeśli nie sprintujemy
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (Character && !Character->GetCharacterMovement()->IsSprinting())
    {
        RegenStamina(DeltaTime);
    }
}

void USurvivalComponent::CheckCriticalLevels()
{
    if (Stats.CurrentHunger <= 0.0f)
    {
        ModifyHealth(-HealthDamagePerSecondWhenStarving * GetWorld()->GetDeltaSeconds());
    }
    if (Stats.CurrentThirst <= 0.0f)
    {
        ModifyHealth(-HealthDamagePerSecondWhenDehydrated * GetWorld()->GetDeltaSeconds());
    }
}

void USurvivalComponent::ModifyHealth(float Delta)
{
    if (bIsDead) return;

    Stats.CurrentHealth = FMath::Clamp(Stats.CurrentHealth + Delta, 0.0f, Stats.MaxHealth);
    OnHealthChanged.Broadcast(Stats.CurrentHealth);

    if (Stats.CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void USurvivalComponent::ModifyHunger(float Delta)
{
    Stats.CurrentHunger = FMath::Clamp(Stats.CurrentHunger + Delta, 0.0f, Stats.MaxHunger);
    OnHungerChanged.Broadcast(Stats.CurrentHunger);
}

void USurvivalComponent::ModifyThirst(float Delta)
{
    Stats.CurrentThirst = FMath::Clamp(Stats.CurrentThirst + Delta, 0.0f, Stats.MaxThirst);
    OnThirstChanged.Broadcast(Stats.CurrentThirst);
}

void USurvivalComponent::ModifyStamina(float Delta)
{
    Stats.CurrentStamina = FMath::Clamp(Stats.CurrentStamina + Delta, 0.0f, Stats.MaxStamina);
    OnStaminaChanged.Broadcast(Stats.CurrentStamina);
}

void USurvivalComponent::ConsumeItem(float HungerRestore, float ThirstRestore, float HealthRestore)
{
    ModifyHunger(HungerRestore);
    ModifyThirst(ThirstRestore);
    ModifyHealth(HealthRestore);
}

void USurvivalComponent::DrainStaminaForSprint(float DeltaTime)
{
    Stats.CurrentStamina -= Stats.StaminaDrainPerSecond * DeltaTime;
    Stats.CurrentStamina = FMath::Clamp(Stats.CurrentStamina, 0.0f, Stats.MaxStamina);
    OnStaminaChanged.Broadcast(Stats.CurrentStamina);
}

void USurvivalComponent::RegenStamina(float DeltaTime)
{
    Stats.CurrentStamina += Stats.StaminaRegenPerSecond * DeltaTime;
    Stats.CurrentStamina = FMath::Clamp(Stats.CurrentStamina, 0.0f, Stats.MaxStamina);
    OnStaminaChanged.Broadcast(Stats.CurrentStamina);
}

void USurvivalComponent::Die()
{
    if (bIsDead) return;
    bIsDead = true;
    OnDeath.Broadcast();

    // Tu: drop ekwipunku, respawn timer, spectator mode
}

void USurvivalComponent::Respawn()
{
    bIsDead = false;
    Stats.CurrentHealth = Stats.MaxHealth;
    Stats.CurrentHunger = Stats.MaxHunger;
    Stats.CurrentThirst = Stats.MaxThirst;
    Stats.CurrentStamina = Stats.MaxStamina;

    OnHealthChanged.Broadcast(Stats.CurrentHealth);
    OnHungerChanged.Broadcast(Stats.CurrentHunger);
    OnThirstChanged.Broadcast(Stats.CurrentThirst);
    OnStaminaChanged.Broadcast(Stats.CurrentStamina);
}

void USurvivalComponent::OnRep_Stats()
{
    OnHealthChanged.Broadcast(Stats.CurrentHealth);
    OnHungerChanged.Broadcast(Stats.CurrentHunger);
    OnThirstChanged.Broadcast(Stats.CurrentThirst);
    OnStaminaChanged.Broadcast(Stats.CurrentStamina);
}

void USurvivalComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(USurvivalComponent, Stats);
    DOREPLIFETIME(USurvivalComponent, bIsDead);
}
