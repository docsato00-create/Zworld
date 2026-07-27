# BEHAVIOR TREE SETUP - Zombie AI

## Wymagane pliki w Content/Blueprints/AI/

### 1. BB_Zombie (Blackboard)
Klucze:
- **TargetActor** (Object - Actor) → aktualny cel
- **HasTarget** (Bool) → czy widzi cel
- **HomeLocation** (Vector) → spawn point
- **InvestigateLocation** (Vector) → ostatni znany hałas
- **ShouldInvestigate** (Bool) → czy zbadać hałas
- **HordeTarget** (Vector) → cel hordy
- **IsInHorde** (Bool) → czy należy do hordy
- **AttackRange** (Float) → zasięg ataku
- **RoamRadius** (Float) → promień wędrówki

### 2. BT_Zombie (Behavior Tree)

```
Root: Selector (Priority)
├── Sequence: Horde Behavior [IsInHorde = true]
│   ├── Move To: HordeTarget (AcceptanceRadius: 200)
│   └── Selector:
│       ├── Sequence: Attack in Range
│       │   ├── BB: TargetActor != null
│       │   ├── Distance to Target < AttackRange
│       │   └── Task: AttackTarget
│       └── Task: Move To: TargetActor
│
├── Sequence: Combat [HasTarget = true]
│   ├── Selector:
│   │   ├── Sequence: Attack
│   │   │   ├── Distance to Target < AttackRange
│   │   │   └── Task: AttackTarget
│   │   └── Task: Move To: TargetActor (AcceptanceRadius: AttackRange - 20)
│   └── Task: Rotate Towards TargetActor
│
├── Sequence: Investigate [ShouldInvestigate = true]
│   ├── Task: Move To: InvestigateLocation (AcceptanceRadius: 100)
│   ├── Wait: 3.0s
│   └── Set: ShouldInvestigate = false
│
└── Sequence: Wander
    ├── Task: Find Random Point in Radius (HomeLocation, RoamRadius)
    ├── Task: Move To: RandomPoint
    └── Wait: 2.0s - 8.0s
```

### 3. BTTask_AttackTarget (Custom Task)
```cpp
// C++ Task
UCLASS()
class UBTTask_AttackTarget : public UBTTaskNode
{
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
    {
        AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
        AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
        AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

        if (Zombie && Target)
        {
            Zombie->AttackTarget(Target);
            return EBTNodeResult::Succeeded;
        }
        return EBTNodeResult::Failed;
    }
};
```

### 4. BTTask_FindRandomPoint (Custom Task)
```cpp
UCLASS()
class UBTTask_FindRandomPoint : public UBTTask_BlackboardBase
{
    UPROPERTY(EditAnywhere)
    float Radius = 3000.0f;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
    {
        UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
        FNavLocation RandomPoint;

        FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector("HomeLocation");

        if (NavSys && NavSys->GetRandomPointInNavigableRadius(Origin, Radius, RandomPoint))
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), RandomPoint.Location);
            return EBTNodeResult::Succeeded;
        }
        return EBTNodeResult::Failed;
    }
};
```

### 5. EQS (Environment Query System) - Opcjonalnie

**EQS_FindNearestPlayer:**
- Generator: Actors of Class (SurvivalCharacter)
- Test: Distance (Score: -1.0, Prefer closer)
- Test: Line of Sight (Filter: Must pass)

**EQS_FindAttackPosition:**
- Generator: Circle (around TargetActor, Radius: AttackRange)
- Test: Distance to Querier (Score: -1.0)
- Test: Trace (Filter: Must have LOS to Target)

## Night Modifier Integration

W `ZombieAIController::Tick()`:
```cpp
if (DayNightManager)
{
    float SpeedMult = DayNightManager->GetZombieSpeedMultiplier();
    if (ZombiePawn)
    {
        ZombiePawn->ApplyTimeOfDayModifier(SpeedMult, SpeedMult);
    }
}
```

## Zombie Types - Behavior Variants

| Typ | Speed | Aggro Range | Behavior |
|-----|-------|-------------|----------|
| Walker | 0.8x | 10m | Standard BT |
| Runner | 1.5x | 15m | Faster MoveTo, shorter Wait |
| Tank | 0.6x | 20m | Higher HP, slower, charges |
| Spitter | 1.0x | 25m | Ranged attack, keeps distance |
| Boss | 1.2x | 30m | Special abilities, summons minions |

## Performance Tips

1. **AI Tick Interval**: Ustaw w `ZombieBase` → `AIController` → `SetTickInterval(0.2)` dla zombie daleko od graczy
2. **LOD for AI**: Zombie >50m od gracza → tick co 1s, no rotation update
3. **Object Pooling**: Użyj pool dla zombie zamiast Spawn/Destroy
4. **NavMesh**: Użyj `Runtime NavMesh` tylko w obrębie 100m od graczy
