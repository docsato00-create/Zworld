// ZombieBase.h
// Source/ZombieSurvival/AI/ZombieBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalTypes.h"
#include "ZombieBase.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AZombieBase : public ACharacter
{
    GENERATED_BODY()

public:
    AZombieBase();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    EZombieType ZombieType = EZombieType::Walker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    float BaseHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    float BaseDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    float BaseSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    float AttackCooldown = 1.5f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zombie")
    float CurrentHealth = 100.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zombie")
    bool bIsDead = false;

    // === NIGHT MODIFIER ===
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    void ApplyTimeOfDayModifier(float SpeedMultiplier, float DamageMultiplier);

    // === COMBAT ===
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    void TakeZombieDamage(float Damage, AActor* DamageDealer);

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    void AttackTarget(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    void Die();

    // === HORDE ===
    UPROPERTY(BlueprintReadWrite, Category = "Zombie")
    bool bIsInHorde = false;

    UPROPERTY(BlueprintReadWrite, Category = "Zombie")
    FVector HordeTargetLocation;

    // === LOOT ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    TArray<TSubclassOf<class UItemData>> PossibleLoot;

protected:
    float CurrentSpeedMultiplier = 1.0f;
    float CurrentDamageMultiplier = 1.0f;
    float LastAttackTime = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Zombie")
    class UAnimInstance* ZombieAnimInstance;

    UFUNCTION()
    void OnRep_Health();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
