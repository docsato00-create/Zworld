// AssembledVehicle.h
// Source/ZombieSurvival/Vehicles/AssembledVehicle.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AssembledVehicle.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API AAssembledVehicle : public APawn
{
    GENERATED_BODY()

public:
    AAssembledVehicle();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void BeginPlay() override;

    // === VEHICLE STATS ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float MaxSpeed = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float Acceleration = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float TurnSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float MaxFuel = 100.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle")
    float CurrentFuel = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float FuelConsumptionPerSecond = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float MaxHealth = 1000.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle")
    float CurrentHealth = 1000.0f;

    // === RUNTIME ===
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle")
    bool bIsOccupied = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle")
    APlayerState* OwnerPlayerState = nullptr;

    // === FUNCTIONS ===
    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void EnterVehicle(ACharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void ExitVehicle(ACharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void Refuel(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void Repair(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void TakeDamage(float Damage);

    UFUNCTION(BlueprintPure, Category = "Vehicle")
    bool HasFuel() const { return CurrentFuel > 0.0f; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class USkeletalMeshComponent* VehicleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class UBoxComponent* VehicleCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class UCameraComponent* VehicleCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    class UFloatingPawnMovement* VehicleMovement;

    UPROPERTY(EditAnywhere, Category = "Vehicle")
    FVector DriverSeatOffset = FVector(0, 0, 100);

    UPROPERTY()
    ACharacter* CurrentDriver = nullptr;

    float CurrentThrottle = 0.0f;
    float CurrentSteering = 0.0f;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void ApplyMovement(float DeltaTime);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
