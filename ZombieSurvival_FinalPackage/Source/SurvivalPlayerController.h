// SurvivalPlayerController.h
// Source/ZombieSurvival/Player/SurvivalPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API ASurvivalPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASurvivalPlayerController();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleInventory();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleCraftingMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleMap();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowDeathScreen();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideDeathScreen();

    UFUNCTION(BlueprintCallable, Category = "Respawn")
    void RequestRespawn();

    UFUNCTION(Client, Reliable)
    void Client_ShowNotification(const FText& Message, float Duration = 3.0f);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> InventoryWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> CraftingWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> DeathScreenWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* CurrentHUD = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* InventoryWidget = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* CraftingWidget = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* DeathScreenWidget = nullptr;

    UFUNCTION(Server, Reliable)
    void Server_RequestRespawn();
};
