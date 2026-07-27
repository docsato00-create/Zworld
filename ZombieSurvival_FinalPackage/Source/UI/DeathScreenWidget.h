// DeathScreenWidget.h
// Source/ZombieSurvival/UI/DeathScreenWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenWidget.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API UDeathScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Death")
    void SetDeathInfo(const FString& KillerName, int32 ZombiesKilled, int32 TimeSurvived);

    UFUNCTION(BlueprintCallable, Category = "Death")
    void OnRespawnClicked();

    UFUNCTION(BlueprintCallable, Category = "Death")
    void OnMainMenuClicked();

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* KillerText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StatsText;

    UPROPERTY(meta = (BindWidget))
    class UButton* RespawnButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* MainMenuButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RespawnTimerText;

    UPROPERTY()
    float RespawnCountdown = 10.0f;

    UPROPERTY()
    FTimerHandle RespawnTimerHandle;

    UFUNCTION()
    void UpdateRespawnTimer();
};
