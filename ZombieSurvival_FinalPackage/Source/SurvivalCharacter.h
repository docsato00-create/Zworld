// SurvivalCharacter.h
// Source/ZombieSurvival/Player/SurvivalCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API ASurvivalCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASurvivalCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void BeginPlay() override;

    // === COMPONENTS ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USurvivalComponent* SurvivalComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UInventoryComponent* InventoryComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBuildingComponent* BuildingComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* FollowCamera;

    // === INPUT ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* FireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* BuildModeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* InventoryAction;

    // === COMBAT ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float InteractionRange = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MeleeDamage = 15.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
    bool bIsAiming = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
    bool bIsSprinting = false;

    // === FUNCTIONS ===
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void PerformInteraction();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void PerformMeleeAttack();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void ToggleBuildMode();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StartSprint();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StopSprint();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool CanSprint() const;

    // === MULTIPLIER ===
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ApplyDamageToCharacter(float Damage, AActor* DamageCauser);

protected:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void FireWeapon();
    void StartAim();
    void StopAim();

    UFUNCTION(Server, Reliable)
    void Server_PerformMeleeAttack();

    UFUNCTION(Server, Reliable)
    void Server_PerformInteraction(AActor* Target);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayMeleeAnim();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
