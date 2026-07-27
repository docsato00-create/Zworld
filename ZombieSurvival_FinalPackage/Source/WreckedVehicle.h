// WreckedVehicle.h
// Source/ZombieSurvival/Vehicles/WreckedVehicle.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalTypes.h"
#include "WreckedVehicle.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AWreckedVehicle : public AActor
{
    GENERATED_BODY()

public:
    AWreckedVehicle();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    FName VehicleName = FName("Buggy");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    TArray<FVehiclePartSlot> RequiredParts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    TSubclassOf<class AAssembledVehicle> AssembledVehicleClass;

    UPROPERTY(ReplicatedUsing = OnRep_AssemblyProgress, BlueprintReadOnly, Category = "Vehicle")
    float AssemblyProgress = 0.0f; // 0-100

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle")
    bool bIsFullyAssembled = false;

    // === INTERACTION ===
    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    bool InstallPart(class UItemData* PartItem, AActor* Installer);

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    bool CanInstallPart(class UItemData* PartItem) const;

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void FinishAssembly();

    UFUNCTION(BlueprintPure, Category = "Vehicle")
    float GetAssemblyPercent() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle")
    bool IsPartRequired(FName PartName) const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class UStaticMeshComponent* WreckMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class USphereComponent* InteractionRadius;

    UFUNCTION()
    void OnRep_AssemblyProgress();

    UFUNCTION()
    void CheckAssemblyComplete();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
