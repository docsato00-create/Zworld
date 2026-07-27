// BuildingPiece.h
// Source/ZombieSurvival/Building/BuildingPiece.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalTypes.h"
#include "BuildingPiece.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API ABuildingPiece : public AActor
{
    GENERATED_BODY()

public:
    ABuildingPiece();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    EBuildingTier BuildingTier = EBuildingTier::Wood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    float MaxHealth = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    float DecayRatePerHour = 10.0f;

    UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Building")
    float CurrentHealth = 500.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Building")
    bool bIsDestroyed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    bool bRequiresClaim = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    FVector GridSize = FVector(200.0f, 200.0f, 100.0f);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void TakeBuildingDamage(float Damage, AActor* DamageCauser);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void Repair(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void DestroyBuilding();

    UFUNCTION(BlueprintPure, Category = "Building")
    float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    class UStaticMeshComponent* BuildingMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    class UBoxComponent* BuildingCollision;

    UFUNCTION()
    void OnRep_Health();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
