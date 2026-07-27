// CraftingRecipe.cpp

#include "CraftingRecipe.h"
#include "InventoryComponent.h"
#include "ItemData.h"

bool UCraftingRecipe::CanCraft(UInventoryComponent* Inventory, int32 PlayerLevel) const
{
    if (!Inventory || PlayerLevel < RequiredLevel) return false;

    for (const FCraftingIngredient& Ingredient : Ingredients)
    {
        if (!Inventory->HasItem(Ingredient.Item, Ingredient.Quantity))
        {
            return false;
        }
    }
    return true;
}
