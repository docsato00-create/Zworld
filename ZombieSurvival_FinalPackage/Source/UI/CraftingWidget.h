// CraftingWidget.h
// Source/ZombieSurvival/UI/CraftingWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingWidget.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API UCraftingWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Crafting UI")
    void RefreshRecipes();

    UFUNCTION(BlueprintCallable, Category = "Crafting UI")
    void SelectRecipe(TSoftObjectPtr<class UCraftingRecipe> Recipe);

    UFUNCTION(BlueprintCallable, Category = "Crafting UI")
    void CraftSelectedRecipe();

    UFUNCTION(BlueprintCallable, Category = "Crafting UI")
    void FilterByCategory(FName Category);

    UFUNCTION(BlueprintCallable, Category = "Crafting UI")
    void SearchRecipes(const FString& SearchText);

    UPROPERTY(BlueprintReadOnly, Category = "Crafting UI")
    TSoftObjectPtr<class UCraftingRecipe> SelectedRecipe;

    UPROPERTY(BlueprintReadOnly, Category = "Crafting UI")
    class UInventoryComponent* PlayerInventory;

protected:
    UPROPERTY(meta = (BindWidget))
    class UScrollBox* RecipeList;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* IngredientsList;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RecipeNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RecipeDescriptionText;

    UPROPERTY(meta = (BindWidget))
    class UButton* CraftButton;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* CraftingProgressBar;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting UI")
    TSubclassOf<class UUserWidget> RecipeEntryClass;

    UPROPERTY()
    TArray<TSoftObjectPtr<class UCraftingRecipe>> AllRecipes;

    UPROPERTY()
    TArray<TSoftObjectPtr<class UCraftingRecipe>> FilteredRecipes;

    UFUNCTION(BlueprintImplementableEvent, Category = "Crafting UI")
    void OnRecipeSelected();

    UFUNCTION(BlueprintImplementableEvent, Category = "Crafting UI")
    void OnCraftingStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Crafting UI")
    void OnCraftingCompleted();
};
