// SurvivalPlayerState.h
// Source/ZombieSurvival/Player/SurvivalPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SurvivalPlayerState.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API ASurvivalPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ASurvivalPlayerState();

    UPROPERTY(ReplicatedUsing = OnRep_PlayerLevel, BlueprintReadOnly, Category = "Progression")
    int32 PlayerLevel = 1;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
    float CurrentXP = 0.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
    float XPToNextLevel = 100.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
    int32 AvailableSkillPoints = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    int32 ZombiesKilled = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    int32 PlayersKilled = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float TimePlayed = 0.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Base")
    FVector LastBaseLocation = FVector::ZeroVector;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Base")
    FDateTime LastLoginTime;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AddXP(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LevelUp();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void SpendSkillPoint(FName SkillName);

protected:
    UFUNCTION()
    void OnRep_PlayerLevel();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
