// SurvivalPlayerState.cpp

#include "SurvivalPlayerState.h"
#include "Net/UnrealNetwork.h"

ASurvivalPlayerState::ASurvivalPlayerState()
{
    bReplicates = true;
}

void ASurvivalPlayerState::AddXP(float Amount)
{
    if (GetLocalRole() != ROLE_Authority) return;

    CurrentXP += Amount;
    if (CurrentXP >= XPToNextLevel)
    {
        LevelUp();
    }
}

void ASurvivalPlayerState::LevelUp()
{
    PlayerLevel++;
    CurrentXP -= XPToNextLevel;
    XPToNextLevel *= 1.5f; // Eksponencjalny wzrost
    AvailableSkillPoints++;

    OnRep_PlayerLevel();
}

void ASurvivalPlayerState::SpendSkillPoint(FName SkillName)
{
    if (AvailableSkillPoints <= 0) return;
    AvailableSkillPoints--;
    // Zastosuj bonus do statystyk
}

void ASurvivalPlayerState::OnRep_PlayerLevel()
{
    // Update UI
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASurvivalPlayerState, PlayerLevel);
    DOREPLIFETIME(ASurvivalPlayerState, CurrentXP);
    DOREPLIFETIME(ASurvivalPlayerState, XPToNextLevel);
    DOREPLIFETIME(ASurvivalPlayerState, AvailableSkillPoints);
    DOREPLIFETIME(ASurvivalPlayerState, ZombiesKilled);
    DOREPLIFETIME(ASurvivalPlayerState, PlayersKilled);
    DOREPLIFETIME(ASurvivalPlayerState, TimePlayed);
    DOREPLIFETIME(ASurvivalPlayerState, LastBaseLocation);
}
