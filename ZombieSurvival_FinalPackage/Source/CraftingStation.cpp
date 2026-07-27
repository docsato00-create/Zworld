// CraftingStation.cpp

#include "CraftingStation.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "CraftingRecipe.h"
#include "InventoryComponent.h"
#include "ItemData.h"

ACraftingStation::ACraftingStation()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    StationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StationMesh"));
    RootComponent = StationMesh;
}

void ACraftingStation::StartCrafting(TSoftObjectPtr<UCraftingRecipe> Recipe, AActor* Crafter)
{
    if (bIsCrafting || !Recipe.IsValid() || !Crafter) return;

    UCraftingRecipe* RecipePtr = Recipe.LoadSynchronous();
    if (!RecipePtr) return;

    UInventoryComponent* InvComp = Crafter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp || !RecipePtr->CanCraft(InvComp, 1)) return; // TODO: player level

    // Odejmij składniki
    for (const FCraftingIngredient& Ingredient : RecipePtr->Ingredients)
    {
        InvComp->RemoveItem(Ingredient.Item, Ingredient.Quantity);
    }

    CurrentRecipe = Recipe;
    CurrentCrafter = Crafter;
    bIsCrafting = true;
    CraftingProgress = 0.0f;

    float CraftTime = RecipePtr->CraftingTime / CraftingSpeedMultiplier;
    GetWorldTimerManager().SetTimer(CraftingTimerHandle, this, &ACraftingStation::OnCraftingComplete, CraftTime, false);
}

void ACraftingStation::CancelCrafting()
{
    if (!bIsCrafting) return;

    GetWorldTimerManager().ClearTimer(CraftingTimerHandle);
    bIsCrafting = false;
    CraftingProgress = 0.0f;

    // TODO: Zwróć składniki
}

void ACraftingStation::OnCraftingComplete()
{
    if (!CurrentRecipe.IsValid() || !CurrentCrafter) return;

    UCraftingRecipe* RecipePtr = CurrentRecipe.LoadSynchronous();
    if (!RecipePtr) return;

    UInventoryComponent* InvComp = CurrentCrafter->FindComponentByClass<UInventoryComponent>();
    if (InvComp)
    {
        InvComp->AddItem(RecipePtr->ResultItem, RecipePtr->ResultQuantity);
    }

    bIsCrafting = false;
    CraftingProgress = 0.0f;
    CurrentRecipe.Reset();
    CurrentCrafter = nullptr;
}

void ACraftingStation::UpdateCraftingProgress()
{
    // Called by tick if needed
}

void ACraftingStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACraftingStation, bIsCrafting);
    DOREPLIFETIME(ACraftingStation, CraftingProgress);
}
