// CraftingRecipe.h
// Source/ZombieSurvival/Inventory/CraftingRecipe.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CraftingRecipe.generated.h"

USTRUCT(BlueprintType)
struct FCraftingIngredient
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<class UItemData> Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity = 1;
};

UCLASS()
class ZOMBIESURVIVAL_API UCraftingRecipe : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    FName RecipeName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    TArray<FCraftingIngredient> Ingredients;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    TSoftObjectPtr<class UItemData> ResultItem;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    int32 ResultQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    float CraftingTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    int32 RequiredLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    TSubclassOf<class AActor> RequiredStation; // nullptr = ręcznie

    UFUNCTION(BlueprintPure, Category = "Crafting")
    bool CanCraft(class UInventoryComponent* Inventory, int32 PlayerLevel) const;
};
