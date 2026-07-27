# OPTIMIZATION GUIDE - Zombie Survival

## 🎯 Performance Targets

| Metric | Minimum | Target | Stress Test |
|--------|---------|--------|-------------|
| FPS (Client) | 30 | 60+ | 50+ |
| Server Tick | 30 Hz | 30 Hz | 30 Hz |
| Zombie Count | 100 | 200 | 500 |
| Players | 20 | 50 | 100 |
| Memory | 4GB | 6GB | 8GB |

---

## 1. ZOMBIE AI OPTIMIZATION

### 1.1 AI Tick Interval
```cpp
// In ZombieAIController::BeginPlay
float DistanceToNearestPlayer = GetDistanceToNearestPlayer();
if (DistanceToNearestPlayer > 5000.0f)
{
    SetActorTickInterval(1.0f); // Far away: tick once per second
}
else if (DistanceToNearestPlayer > 2000.0f)
{
    SetActorTickInterval(0.5f); // Medium: twice per second
}
else
{
    SetActorTickInterval(0.033f); // Close: full 30Hz
}
```

### 1.2 AI LOD System
```cpp
UENUM()
enum class EAILOD : uint8
{
    Full,       // 0-20m: Full AI, animation, collision
    Reduced,    // 20-50m: Reduced tick, simple movement
    Distant,    // 50-100m: Minimal tick, no animation updates
    Frozen      // 100m+: Frozen, no tick, visual only
};

void AZombieBase::UpdateAILOD()
{
    float Dist = GetDistanceToNearestPlayer();
    EAILOD NewLOD = Dist < 2000.0f ? EAILOD::Full :
                    Dist < 5000.0f ? EAILOD::Reduced :
                    Dist < 10000.0f ? EAILOD::Distant : EAILOD::Frozen;

    if (NewLOD != CurrentLOD)
    {
        ApplyLOD(NewLOD);
    }
}

void AZombieBase::ApplyLOD(EAILOD LOD)
{
    switch (LOD)
    {
        case EAILOD::Full:
            GetCharacterMovement()->SetComponentTickEnabled(true);
            GetMesh()->SetComponentTickEnabled(true);
            SetActorTickInterval(0.033f);
            break;
        case EAILOD::Reduced:
            GetCharacterMovement()->SetComponentTickEnabled(true);
            GetMesh()->SetComponentTickEnabled(false);
            SetActorTickInterval(0.2f);
            break;
        case EAILOD::Distant:
            GetCharacterMovement()->SetComponentTickEnabled(false);
            GetMesh()->SetComponentTickEnabled(false);
            SetActorTickInterval(1.0f);
            break;
        case EAILOD::Frozen:
            GetCharacterMovement()->SetComponentTickEnabled(false);
            GetMesh()->SetComponentTickEnabled(false);
            SetActorTickInterval(5.0f);
            break;
    }
}
```

### 1.3 Zombie Object Pooling
```cpp
UCLASS()
class ZOMBIESURVIVAL_API AZombiePool : public AActor
{
    UPROPERTY()
    TArray<AZombieBase*> PooledZombies;

    UPROPERTY()
    TArray<AZombieBase*> ActiveZombies;

    UPROPERTY(EditAnywhere)
    int32 PoolSize = 200;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AZombieBase> ZombieClass;

public:
    AZombieBase* AcquireZombie();
    void ReleaseZombie(AZombieBase* Zombie);
    void InitializePool();
};

AZombieBase* AZombiePool::AcquireZombie()
{
    if (PooledZombies.Num() > 0)
    {
        AZombieBase* Zombie = PooledZombies.Pop();
        Zombie->SetActorHiddenInGame(false);
        Zombie->SetActorEnableCollision(true);
        ActiveZombies.Add(Zombie);
        return Zombie;
    }
    return nullptr; // Pool exhausted
}

void AZombiePool::ReleaseZombie(AZombieBase* Zombie)
{
    if (Zombie)
    {
        Zombie->SetActorHiddenInGame(true);
        Zombie->SetActorEnableCollision(false);
        Zombie->SetActorLocation(FVector(0, 0, -10000)); // Hide under map
        ActiveZombies.Remove(Zombie);
        PooledZombies.Add(Zombie);
    }
}
```

---

## 2. NETWORK OPTIMIZATION

### 2.1 Net Cull Distance
```cpp
// In DefaultEngine.ini
[/Script/OnlineSubsystemUtils.IpNetDriver]
NetServerMaxTickRate=30
MaxNetTickRate=120
MaxInternetClientRate=15000

[/Script/Engine.NetworkSettings]
+NetworkEmulationProfiles=(ProfileName="BadWiFi",PacketLossPercentage=2.0,LatencyInMS=150,JitterInMS=50)
```

### 2.2 Replication Graph
```cpp
// SurvivalReplicationGraph.h
UCLASS()
class ZOMBIESURVIVAL_API USurvivalReplicationGraph : public UReplicationGraph
{
    UPROPERTY()
    UReplicationGraphNode_GridSpatialization2D* ZombieGridNode;

    UPROPERTY()
    UReplicationGraphNode_GridSpatialization2D* BuildingGridNode;

    UPROPERTY()
    UReplicationGraphNode_ActorList* AlwaysRelevantNode;
};

void USurvivalReplicationGraph::InitGlobalActorClassSettings()
{
    Super::InitGlobalActorClassSettings();

    // Zombies replicate only within 10m
    auto ZombieSetting = GlobalActorReplicationInfoMap.GetSettings(AZombieBase::StaticClass());
    ZombieSetting->SetCullDistanceSquared(1000000.0f); // 1000m

    // Buildings replicate within 5km
    auto BuildingSetting = GlobalActorReplicationInfoMap.GetSettings(ABuildingPiece::StaticClass());
    BuildingSetting->SetCullDistanceSquared(25000000.0f); // 5000m

    // Vehicles replicate within 10km
    auto VehicleSetting = GlobalActorReplicationInfoMap.GetSettings(AAssembledVehicle::StaticClass());
    VehicleSetting->SetCullDistanceSquared(100000000.0f); // 10000m
}
```

### 2.3 Conditional Replication
```cpp
// In ZombieBase.h
UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category="Zombie")
float CurrentHealth = 100.0f;

virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override
{
    // Don't replicate to players who can't see us
    const float DistSq = FVector::DistSquared(SrcLocation, GetActorLocation());
    return DistSq < NetCullDistanceSquared;
}
```

### 2.4 Network Prediction
```cpp
// For movement - use CharacterMovementComponent's built-in prediction
// For shooting - client-side prediction with server reconciliation

UFUNCTION(Client, Reliable)
void Client_FireConfirmed(int32 ShotID, bool bHit, FVector HitLocation);

UFUNCTION(Client, Reliable)
void Client_FireRejected(int32 ShotID);
```

---

## 3. RENDERING OPTIMIZATION

### 3.1 LOD for Meshes
| Distance | LOD | Triangle Budget |
|----------|-----|----------------|
| 0-10m | LOD0 | 5000 |
| 10-30m | LOD1 | 2500 |
| 30-100m | LOD2 | 1000 |
| 100m+ | LOD3 | 500 |

### 3.2 HLOD (Hierarchical LOD)
```
// In World Settings
Enable HLOD: true
HLOD Distance: 5000
HLOD Mesh: Generated proxy mesh for building clusters
```

### 3.3 Occlusion Culling
```ini
[/Script/Engine.RendererSettings]
r.AllowOcclusionQueries=1
r.OcclusionQueryLocation=1
r.HZBOcclusion=1
```

### 3.4 Shadow Optimization
```ini
[/Script/Engine.RendererSettings]
r.Shadow.CSM.MaxCascades=3
r.Shadow.DistanceScale=0.7
r.Shadow.CSM.TransitionScale=0.8
r.Shadow.Virtual.Enable=1
```

### 3.5 Post Process Optimization
```ini
[/Script/Engine.RendererSettings]
r.PostProcessAAQuality=3
r.BloomQuality=3
r.LensFlareQuality=0
r.MotionBlurQuality=0
r.SceneColorFringeQuality=0
```

---

## 4. MEMORY OPTIMIZATION

### 4.1 Texture Streaming
```ini
[/Script/Engine.RendererSettings]
r.Streaming.PoolSize=2048
r.Streaming.LimitPoolSizeToVRAM=1
r.Streaming.MaxTempMemoryAllowed=512
```

### 4.2 Garbage Collection
```cpp
// Force GC during loading screens
void ASurvivalGameMode::ForceGarbageCollection()
{
    GEngine->ForceGarbageCollection(true);
}
```

### 4.3 Soft References
```cpp
// Use TSoftObjectPtr for assets that aren't always needed
UPROPERTY(EditAnywhere, BlueprintReadOnly)
TSoftObjectPtr<UStaticMesh> HighDetailMesh;

// Load only when needed
UStaticMesh* Mesh = HighDetailMesh.LoadSynchronous();
```

---

## 5. PHYSICS OPTIMIZATION

### 5.1 Async Physics
```ini
[/Script/Engine.PhysicsSettings]
bTickPhysicsAsync=True
AsyncFixedTimeStepSize=0.016666
MaxPhysicsDeltaTime=0.033333
```

### 5.2 Simple Collision for Buildings
- Use Box collision (not complex mesh)
- Merge adjacent building pieces into single collision body

### 5.3 Vehicle Physics
```cpp
// Use simple physics for far vehicles
if (DistanceToPlayer > 5000.0f)
{
    VehicleMesh->SetSimulatePhysics(false);
    VehicleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
```

---

## 6. PROFILING CHECKLIST

### Editor Profiling
1. **Stat FPS** — Check frame time
2. **Stat Unit** — Game/Draw/GPU thread times
3. **Stat Game** — Game thread details
4. **Stat Memory** — Memory usage
5. **Stat Net** — Network traffic
6. **Frontend** → **Session Frontend** → **Profiler**

### Command Line Profiling
```
YourGame.exe -trace=cpu,gpu,frame,memory,net
```

### Target Budgets (per frame at 60 FPS = 16.67ms)
| System | Budget |
|--------|--------|
| Game Thread | 8ms |
| Render Thread | 5ms |
| GPU | 14ms |
| Network | 1ms |
| Physics | 2ms |
| AI | 2ms |

---

## 7. CONSOLE COMMANDS (In-Game)

| Command | Description |
|---------|-------------|
| `stat fps` | Show FPS |
| `stat unit` | Show thread times |
| `stat game` | Game thread breakdown |
| `stat net` | Network stats |
| `stat ai` | AI performance |
| `stat memory` | Memory usage |
| `show collision` | Toggle collision visibility |
| `show navigation` | Toggle navmesh |
| `show bounds` | Show actor bounds |
| `r.screenpercentage 50` | Render at 50% resolution |
| `r.vsync 0` | Disable VSync |
| `slomo 0.5` | Slow motion (debug) |
