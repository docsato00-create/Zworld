// SurvivalHUDWidget.h
// Source/ZombieSurvival/UI/SurvivalHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalHUDWidget.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API USurvivalHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealthBar(float Percent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHungerBar(float Percent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateThirstBar(float Percent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStaminaBar(float Percent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmo(int32 Current, int32 Max);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowHitMarker();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDamageIndicator(FVector DamageSource);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNotification(const FText& Message, float Duration);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateCompass(float Yaw);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateTimeOfDay(const FString& TimeString, bool bIsNight);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateCrosshair(bool bIsAiming);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HungerBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ThirstBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* StaminaBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AmmoText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TimeText;

    UPROPERTY(meta = (BindWidget))
    class UImage* HitMarker;

    UPROPERTY(meta = (BindWidget))
    class UImage* DamageIndicator;

    UPROPERTY(meta = (BindWidget))
    class UImage* Crosshair;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* NotificationBox;

    UPROPERTY(EditAnywhere, Category = "HUD")
    float HitMarkerDuration = 0.3f;

    UPROPERTY()
    FTimerHandle HitMarkerTimer;

    UFUNCTION()
    void HideHitMarker();

    UPROPERTY()
    class USurvivalComponent* SurvivalComp;

    UPROPERTY()
    class ADayNightManager* DayNightManager;
};
