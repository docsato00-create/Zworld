// CraftingStation.h
// Source/ZombieSurvival/Inventory/CraftingStation.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CraftingStation.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API ACraftingStation : public AActor
{
    GENERATED_BODY()

public:
    ACraftingStation();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
    FName StationName = FName("Workbench");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
    TArray<TSoftObjectPtr<class UCraftingRecipe>> AvailableRecipes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
    float CraftingSpeedMultiplier = 1.0f;

    UFUNCTION(BlueprintCallable, Category = "Crafting")
    void StartCrafting(TSoftObjectPtr<class UCraftingRecipe> Recipe, AActor* Crafter);

    UFUNCTION(BlueprintCallable, Category = "Crafting")
    void CancelCrafting();

    UFUNCTION(BlueprintPure, Category = "Crafting")
    bool IsCrafting() const { return bIsCrafting; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
    class UStaticMeshComponent* StationMesh;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Crafting")
    bool bIsCrafting = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Crafting")
    float CraftingProgress = 0.0f;

    UPROPERTY()
    TSoftObjectPtr<UCraftingRecipe> CurrentRecipe;

    UPROPERTY()
    AActor* CurrentCrafter = nullptr;

    UPROPERTY()
    FTimerHandle CraftingTimerHandle;

    UFUNCTION()
    void OnCraftingComplete();

    UFUNCTION()
    void UpdateCraftingProgress();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
