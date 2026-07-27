// CraftingWidget.cpp

#include "CraftingWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "InventoryComponent.h"
#include "CraftingRecipe.h"
#include "SurvivalCharacter.h"
#include "ItemData.h"

void UCraftingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOwningPlayerPawn()))
    {
        PlayerInventory = Character->FindComponentByClass<UInventoryComponent>();
    }

    RefreshRecipes();
}

void UCraftingWidget::RefreshRecipes()
{
    // Load all CraftingRecipe DataAssets
    // Filter by CanCraft
    // Populate RecipeList
}

void UCraftingWidget::SelectRecipe(TSoftObjectPtr<UCraftingRecipe> Recipe)
{
    SelectedRecipe = Recipe;
    if (RecipeNameText && Recipe.IsValid())
    {
        UCraftingRecipe* RecipePtr = Recipe.LoadSynchronous();
        if (RecipePtr)
        {
            RecipeNameText->SetText(RecipePtr->DisplayName);
            // Show ingredients
        }
    }
    OnRecipeSelected();
}

void UCraftingWidget::CraftSelectedRecipe()
{
    if (!SelectedRecipe.IsValid() || !PlayerInventory) return;

    UCraftingRecipe* RecipePtr = SelectedRecipe.LoadSynchronous();
    if (!RecipePtr) return;

    // Check ingredients again
    if (RecipePtr->CanCraft(PlayerInventory, 1)) // TODO: player level
    {
        OnCraftingStarted();
        // Start crafting timer
        // On completion: OnCraftingCompleted()
    }
}

void UCraftingWidget::FilterByCategory(FName Category)
{
    // Filter AllRecipes by category
}

void UCraftingWidget::SearchRecipes(const FString& SearchText)
{
    // Filter by search text
}
