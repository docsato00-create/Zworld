// ZombieAIController.cpp

#include "ZombieAIController.h"
#include "ZombieBase.h"
#include "SurvivalCharacter.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AZombieAIController::AZombieAIController()
{
    // Setup Perception
    UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    PerceptionComp->ConfigureSense(*SightConfig);

    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = HearingRadius;
    PerceptionComp->ConfigureSense(*HearingConfig);

    PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ZombiePawn = Cast<AZombieBase>(InPawn);
    if (ZombiePawn)
    {
        HomeLocation = InPawn->GetActorLocation();

        // Run Behavior Tree
        UBehaviorTree* BT = ZombiePawn->BehaviorTree;
        if (BT)
        {
            RunBehaviorTree(BT);
            Blackboard->SetValueAsVector(FName("HomeLocation"), HomeLocation);
            Blackboard->SetValueAsFloat(FName("AttackRange"), AttackRange);
            Blackboard->SetValueAsFloat(FName("RoamRadius"), RoamRadius);
        }
    }
}

void AZombieAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentTarget)
    {
        float TimeSinceSeen = GetWorld()->GetTimeSeconds() - LastTargetSeenTime;
        if (TimeSinceSeen > TargetMemoryDuration)
        {
            ClearTarget();
        }
    }

    // Update Blackboard
    if (Blackboard)
    {
        Blackboard->SetValueAsObject(FName("TargetActor"), CurrentTarget);
        Blackboard->SetValueAsBool(FName("HasTarget"), HasTarget());
        Blackboard->SetValueAsVector(FName("HordeTarget"), HordeTargetLocation);
        Blackboard->SetValueAsBool(FName("IsInHorde"), ZombiePawn ? ZombiePawn->bIsInHorde : false);
    }
}

void AZombieAIController::SetTarget(AActor* NewTarget)
{
    CurrentTarget = NewTarget;
    LastTargetSeenTime = GetWorld()->GetTimeSeconds();
}

void AZombieAIController::ClearTarget()
{
    CurrentTarget = nullptr;
}

void AZombieAIController::SetHordeTarget(FVector Location)
{
    HordeTargetLocation = Location;
}

void AZombieAIController::UpdateSenses()
{
    // Manual sense update if needed
}

void AZombieAIController::OnSensePlayer(AActor* SensedActor)
{
    if (ASurvivalCharacter* Player = Cast<ASurvivalCharacter>(SensedActor))
    {
        SetTarget(Player);
    }
}

void AZombieAIController::OnHearNoise(FVector NoiseLocation, float Loudness)
{
    // Investigate noise
    if (!HasTarget() && Blackboard)
    {
        Blackboard->SetValueAsVector(FName("InvestigateLocation"), NoiseLocation);
        Blackboard->SetValueAsBool(FName("ShouldInvestigate"), true);
    }
}
