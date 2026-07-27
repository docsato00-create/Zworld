// SurvivalComponent.h
// Source/ZombieSurvival/Player/SurvivalComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalTypes.h"
#include "SurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZOMBIESURVIVAL_API USurvivalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USurvivalComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // === GETTERS ===
    UFUNCTION(BlueprintPure, Category = "Survival")
    float GetHealthPercent() const { return Stats.CurrentHealth / Stats.MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Survival")
    float GetHungerPercent() const { return Stats.CurrentHunger / Stats.MaxHunger; }

    UFUNCTION(BlueprintPure, Category = "Survival")
    float GetThirstPercent() const { return Stats.CurrentThirst / Stats.MaxThirst; }

    UFUNCTION(BlueprintPure, Category = "Survival")
    float GetStaminaPercent() const { return Stats.CurrentStamina / Stats.MaxStamina; }

    // === MODIFIERS ===
    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ModifyHealth(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ModifyHunger(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ModifyThirst(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ModifyStamina(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ConsumeItem(float HungerRestore, float ThirstRestore, float HealthRestore);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void DrainStaminaForSprint(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void RegenStamina(float DeltaTime);

    // === DEATH ===
    UFUNCTION(BlueprintCallable, Category = "Survival")
    void Die();

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void Respawn();

    // === REPLICATION ===
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // === DELEGATES ===
    UPROPERTY(BlueprintAssignable, Category = "Survival")
    FOnStatChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival")
    FOnStatChanged OnHungerChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival")
    FOnStatChanged OnThirstChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival")
    FOnStatChanged OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival")
    FOnPlayerDeath OnDeath;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival", ReplicatedUsing = OnRep_Stats)
    FSurvivalStats Stats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float HealthDamagePerSecondWhenStarving = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
    float HealthDamagePerSecondWhenDehydrated = 3.0f;

    UFUNCTION()
    void OnRep_Stats();

    void ApplyPassiveDecay(float DeltaTime);
    void CheckCriticalLevels();

    UPROPERTY(Replicated)
    bool bIsDead = false;
};
