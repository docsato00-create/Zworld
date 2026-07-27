// SurvivalHUDWidget.cpp

#include "SurvivalHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "SurvivalComponent.h"
#include "DayNightManager.h"
#include "SurvivalCharacter.h"
#include "Kismet/GameplayStatics.h"

void USurvivalHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOwningPlayerPawn()))
    {
        SurvivalComp = Character->FindComponentByClass<USurvivalComponent>();
        if (SurvivalComp)
        {
            SurvivalComp->OnHealthChanged.AddDynamic(this, &USurvivalHUDWidget::UpdateHealthBar);
            SurvivalComp->OnHungerChanged.AddDynamic(this, &USurvivalHUDWidget::UpdateHungerBar);
            SurvivalComp->OnThirstChanged.AddDynamic(this, &USurvivalHUDWidget::UpdateThirstBar);
            SurvivalComp->OnStaminaChanged.AddDynamic(this, &USurvivalHUDWidget::UpdateStaminaBar);
        }
    }

    TArray<AActor*> DayNightManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightManager::StaticClass(), DayNightManagers);
    if (DayNightManagers.Num() > 0)
    {
        DayNightManager = Cast<ADayNightManager>(DayNightManagers[0]);
        if (DayNightManager)
        {
            DayNightManager->OnTimeChanged.AddDynamic(this, &USurvivalHUDWidget::UpdateTimeOfDay);
        }
    }

    if (HitMarker) HitMarker->SetVisibility(ESlateVisibility::Hidden);
    if (DamageIndicator) DamageIndicator->SetVisibility(ESlateVisibility::Hidden);
}

void USurvivalHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (DayNightManager && TimeText)
    {
        TimeText->SetText(FText::FromString(DayNightManager->GetFormattedTime()));
    }
}

void USurvivalHUDWidget::UpdateHealthBar(float Percent)
{
    if (HealthBar) HealthBar->SetPercent(Percent);
}

void USurvivalHUDWidget::UpdateHungerBar(float Percent)
{
    if (HungerBar) HungerBar->SetPercent(Percent);
}

void USurvivalHUDWidget::UpdateThirstBar(float Percent)
{
    if (ThirstBar) ThirstBar->SetPercent(Percent);
}

void USurvivalHUDWidget::UpdateStaminaBar(float Percent)
{
    if (StaminaBar) StaminaBar->SetPercent(Percent);
}

void USurvivalHUDWidget::UpdateAmmo(int32 Current, int32 Max)
{
    if (AmmoText)
    {
        AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Current, Max)));
    }
}

void USurvivalHUDWidget::ShowHitMarker()
{
    if (HitMarker)
    {
        HitMarker->SetVisibility(ESlateVisibility::Visible);
        GetWorld()->GetTimerManager().SetTimer(HitMarkerTimer, this, &USurvivalHUDWidget::HideHitMarker, HitMarkerDuration, false);
    }
}

void USurvivalHUDWidget::HideHitMarker()
{
    if (HitMarker) HitMarker->SetVisibility(ESlateVisibility::Hidden);
}

void USurvivalHUDWidget::ShowDamageIndicator(FVector DamageSource)
{
    // Calculate angle and rotate indicator
}

void USurvivalHUDWidget::ShowNotification(const FText& Message, float Duration)
{
    // Create notification widget and add to NotificationBox
}

void USurvivalHUDWidget::UpdateCompass(float Yaw)
{
    // Rotate compass based on player yaw
}

void USurvivalHUDWidget::UpdateTimeOfDay(const FString& TimeString, bool bIsNight)
{
    if (TimeText)
    {
        FLinearColor Color = bIsNight ? FLinearColor(0.2f, 0.2f, 0.8f) : FLinearColor(1.0f, 0.8f, 0.2f);
        TimeText->SetColorAndOpacity(Color);
    }
}

void USurvivalHUDWidget::UpdateCrosshair(bool bIsAiming)
{
    if (Crosshair)
    {
        Crosshair->SetVisibility(bIsAiming ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}
