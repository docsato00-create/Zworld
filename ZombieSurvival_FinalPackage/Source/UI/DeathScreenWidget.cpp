// DeathScreenWidget.cpp

#include "DeathScreenWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "SurvivalPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UDeathScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RespawnButton)
    {
        RespawnButton->OnClicked.AddDynamic(this, &UDeathScreenWidget::OnRespawnClicked);
    }
    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UDeathScreenWidget::OnMainMenuClicked);
    }

    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UDeathScreenWidget::UpdateRespawnTimer, 1.0f, true);
}

void UDeathScreenWidget::SetDeathInfo(const FString& KillerName, int32 ZombiesKilled, int32 TimeSurvived)
{
    if (KillerText)
    {
        KillerText->SetText(FText::FromString(FString::Printf(TEXT("Zabity przez: %s"), *KillerName)));
    }
    if (StatsText)
    {
        StatsText->SetText(FText::FromString(FString::Printf(TEXT("Zombie zabitych: %d\nCzas przetrwania: %d min"), ZombiesKilled, TimeSurvived)));
    }
}

void UDeathScreenWidget::UpdateRespawnTimer()
{
    RespawnCountdown -= 1.0f;
    if (RespawnTimerText)
    {
        RespawnTimerText->SetText(FText::FromString(FString::Printf(TEXT("Respawn za: %.0f"), RespawnCountdown)));
    }

    if (RespawnCountdown <= 0.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
        OnRespawnClicked();
    }
}

void UDeathScreenWidget::OnRespawnClicked()
{
    if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController>(GetOwningPlayer()))
    {
        PC->RequestRespawn();
    }
    RemoveFromViewport();
}

void UDeathScreenWidget::OnMainMenuClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
}
