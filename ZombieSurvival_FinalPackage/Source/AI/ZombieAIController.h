// ZombieAIController.h
// Source/ZombieSurvival/AI/ZombieAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    AZombieAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void SetTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void ClearTarget();

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void SetHordeTarget(FVector Location);

    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    bool HasTarget() const { return CurrentTarget != nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void UpdateSenses();

protected:
    UPROPERTY()
    class AZombieBase* ZombiePawn;

    UPROPERTY()
    AActor* CurrentTarget = nullptr;

    UPROPERTY()
    FVector HordeTargetLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Zombie AI")
    float SightRadius = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Zombie AI")
    float LoseSightRadius = 2000.0f;

    UPROPERTY(EditAnywhere, Category = "Zombie AI")
    float HearingRadius = 800.0f;

    UPROPERTY(EditAnywhere, Category = "Zombie AI")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, Category = "Zombie AI")
    float RoamRadius = 3000.0f;

    UPROPERTY()
    FVector HomeLocation = FVector::ZeroVector;

    UPROPERTY()
    float LastTargetSeenTime = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Zombie AI")
    float TargetMemoryDuration = 5.0f;

    UFUNCTION()
    void OnSensePlayer(AActor* SensedActor);

    UFUNCTION()
    void OnHearNoise(FVector NoiseLocation, float Loudness);
};
