// ClaimBlock.h
// Source/ZombieSurvival/Building/ClaimBlock.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClaimBlock.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AClaimBlock : public AActor
{
    GENERATED_BODY()

public:
    AClaimBlock();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // === CONFIG ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Claim")
    float ClaimRadius = 3000.0f; // 30m

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Claim")
    float MaxHealth = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Claim")
    float DecayRatePerHour = 50.0f; // HP tracone na godzinę offline

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Claim")
    int32 MaxUpgradeLevel = 5;

    // === RUNTIME ===
    UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Claim")
    float CurrentHealth = 5000.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Claim")
    int32 UpgradeLevel = 1;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Claim")
    APlayerState* OwnerPlayerState = nullptr;

    // === FUNCTIONS ===
    UFUNCTION(BlueprintCallable, Category = "Claim")
    void TakeDamage(float Damage, AActor* DamageCauser);

    UFUNCTION(BlueprintCallable, Category = "Claim")
    void Upgrade();

    UFUNCTION(BlueprintCallable, Category = "Claim")
    bool CanBuildHere(AActor* Builder) const;

    UFUNCTION(BlueprintCallable, Category = "Claim")
    bool IsWithinClaim(FVector Location) const;

    UFUNCTION(BlueprintCallable, Category = "Claim")
    void ApplyDecay(float DeltaHours);

    // === VISUALS ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Claim")
    class UStaticMeshComponent* BlockMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Claim")
    class USphereComponent* ClaimRadiusSphere;

protected:
    UFUNCTION()
    void OnRep_Health();

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
