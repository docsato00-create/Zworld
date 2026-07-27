// SurvivalPlayerController.cpp

#include "SurvivalPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalCharacter.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
    bShowMouseCursor = false;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && HUDWidgetClass)
    {
        CurrentHUD = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (CurrentHUD)
        {
            CurrentHUD->AddToViewport();
        }
    }
}

void ASurvivalPlayerController::ToggleInventory()
{
    if (!InventoryWidgetClass) return;

    if (!InventoryWidget)
    {
        InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
    }

    if (InventoryWidget->IsInViewport())
    {
        InventoryWidget->RemoveFromViewport();
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());
    }
    else
    {
        InventoryWidget->AddToViewport();
        bShowMouseCursor = true;
        SetInputMode(FInputModeGameAndUI());
    }
}

void ASurvivalPlayerController::ToggleCraftingMenu()
{
    if (!CraftingWidgetClass) return;

    if (!CraftingWidget)
    {
        CraftingWidget = CreateWidget<UUserWidget>(this, CraftingWidgetClass);
    }

    if (CraftingWidget->IsInViewport())
    {
        CraftingWidget->RemoveFromViewport();
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());
    }
    else
    {
        CraftingWidget->AddToViewport();
        bShowMouseCursor = true;
        SetInputMode(FInputModeGameAndUI());
    }
}

void ASurvivalPlayerController::ToggleMap()
{
    // Implementacja mapy
}

void ASurvivalPlayerController::ShowDeathScreen()
{
    if (!DeathScreenWidgetClass) return;

    if (!DeathScreenWidget)
    {
        DeathScreenWidget = CreateWidget<UUserWidget>(this, DeathScreenWidgetClass);
    }

    DeathScreenWidget->AddToViewport();
    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());
}

void ASurvivalPlayerController::HideDeathScreen()
{
    if (DeathScreenWidget && DeathScreenWidget->IsInViewport())
    {
        DeathScreenWidget->RemoveFromViewport();
    }
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());
}

void ASurvivalPlayerController::RequestRespawn()
{
    Server_RequestRespawn();
}

void ASurvivalPlayerController::Server_RequestRespawn_Implementation()
{
    if (GetPawn())
    {
        GetPawn()->Destroy();
    }

    UnPossess();

    FTimerHandle RespawnTimer;
    GetWorldTimerManager().SetTimer(RespawnTimer, [this]()
    {
        if (GetWorld())
        {
            GetWorld()->GetAuthGameMode()->RestartPlayer(this);
        }
    }, 5.0f, false);
}

void ASurvivalPlayerController::Client_ShowNotification_Implementation(const FText& Message, float Duration)
{
    // Wyświetl notification widget
}
