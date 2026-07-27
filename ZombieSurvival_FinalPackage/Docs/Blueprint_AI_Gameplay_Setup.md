# BLUEPRINT AI & GAMEPLAY SETUP

---

## 1. BP_Zombie (Blueprint z C++ klasy ZombieBase)

### Components:
```
Root: CapsuleComponent
├── Mesh (SkeletalMeshComponent)
│   └── Skeletal Mesh: SK_Zombie_Walker (lub placeholder)
├── HealthBarWidget (WidgetComponent)
│   └── Widget Class: WBP_ZombieHealthBar
├── AudioComponent
│   └── Sound: Ambient zombie groan (loop, random intervals)
└── ParticleSystemComponent
    └── Template: P_ZombieBlood (on hit)
```

### Variables:
| Name | Type | Default | Description |
|------|------|---------|-------------|
| ZombieType | EZombieType | Walker | Typ zombie |
| BehaviorTree | BehaviorTree | BT_Zombie | Referencja do BT |
| AttackMontage | AnimMontage | AM_ZombieAttack | Animacja ataku |
| DeathMontage | AnimMontage | AM_ZombieDeath | Animacja śmierci |
| HitReactionMontage | AnimMontage | AM_ZombieHit | Reakcja na hit |
| GroanSounds | Array<SoundBase> | — | Dźwięki losowe |
| AttackSound | SoundBase | — | Dźwięk ataku |
| DeathSound | SoundBase | — | Dźwięk śmierci |
| BloodEffect | ParticleSystem | P_ZombieBlood | Efekt krwi |
| LootTable | DataTable | DT_ZombieLoot | Tabela lootu |

### Event Graph:

**BeginPlay**
```
→ Super::BeginPlay()
→ Set random walk speed variation: BaseSpeed * Rand(0.9, 1.1)
→ Start "AmbientGroan" timer: Rand(5, 15) seconds
→ If bIsInHorde:
    → SetMaterialParameter("HordeGlow", true) // subtle red glow
```

**TakeZombieDamage** (Override)
```
→ Super::TakeZombieDamage(Damage, DamageDealer)
→ Play HitReactionMontage
→ Spawn BloodEffect at hit location
→ Play random pain sound
→ Update HealthBarWidget
→ If Health < 30%:
    → PlayAnimation("Limping") // slower movement
→ If bIsDead:
    → Disable collision
    → Play DeathMontage
    → Play DeathSound
    → Spawn loot (from LootTable)
    → SetLifeSpan(300) // disappear after 5 min
    → If bIsInHorde:
        → Notify HordeManager (member died)
```

**AttackTarget** (Override)
```
→ If Target != null and !bIsDead:
    → Face Target (RotateTowards)
    → Play AttackMontage
    → After 0.3s (hit frame):
        → If DistanceTo(Target) <= AttackRange:
            → Target.ApplyDamage(BaseDamage * CurrentDamageMultiplier)
            → Spawn blood/decal at target
            → Play AttackSound
```

**AmbientGroan** (Timer)
```
→ If !bIsDead:
    → Play random sound from GroanSounds
    → Restart timer: Rand(8, 20) seconds
```

---

## 2. BB_Zombie (Blackboard Data Asset)

### Blackboard Keys:
| Key Name | Type | Description |
|----------|------|-------------|
| TargetActor | Object (Actor) | Aktualny cel (gracz) |
| HasTarget | Bool | Czy ma cel |
| HomeLocation | Vector | Punkt spawnu |
| InvestigateLocation | Vector | Ostatni hałas |
| ShouldInvestigate | Bool | Czy zbadać hałas |
| HordeTarget | Vector | Cel hordy |
| IsInHorde | Bool | Czy w hordzie |
| AttackRange | Float | Zasięg ataku |
| RoamRadius | Float | Promień wędrówki |
| RandomPatrolPoint | Vector | Losowy punkt patrolu |

---

## 3. BT_Zombie (Behavior Tree)

### Root: Selector ("RootSelector")

```
RootSelector
├── Sequence: "Horde Behavior"
│   ├── Blackboard Condition: IsInHorde == true
│   ├── Selector: "Horde Combat"
│   │   ├── Sequence: "Attack Target in Range"
│   │   │   ├── Blackboard Condition: TargetActor != null
│   │   │   ├── BTTask_CalculateDistance (to TargetActor)
│   │   │   ├── Blackboard Condition: Distance <= AttackRange
│   │   │   └── BTTask_AttackTarget (Custom C++ Task)
│   │   └── Sequence: "Move to Target"
│   │       ├── Blackboard Condition: TargetActor != null
│   │       └── BTTask_MoveTo (TargetActor, AcceptanceRadius: AttackRange - 20)
│   └── BTTask_MoveTo (HordeTarget, AcceptanceRadius: 200)
│
├── Sequence: "Combat"
│   ├── Blackboard Condition: HasTarget == true
│   ├── Selector: "Attack or Chase"
│   │   ├── Sequence: "Attack"
│   │   │   ├── BTTask_CalculateDistance (to TargetActor)
│   │   │   ├── Blackboard Condition: Distance <= AttackRange
│   │   │   ├── BTTask_RotateTowards (TargetActor)
│   │   │   └── BTTask_AttackTarget
│   │   └── Sequence: "Chase"
│   │       ├── BTTask_RotateTowards (TargetActor)
│   │       └── BTTask_MoveTo (TargetActor, AcceptanceRadius: AttackRange - 20)
│   └── BTTask_Wait (0.5) // prevent tick spam
│
├── Sequence: "Investigate"
│   ├── Blackboard Condition: ShouldInvestigate == true
│   ├── BTTask_MoveTo (InvestigateLocation, AcceptanceRadius: 100)
│   ├── BTTask_Wait (Rand(2, 5))
│   └── BTTask_SetBlackboard (ShouldInvestigate = false)
│
└── Sequence: "Wander"
    ├── BTTask_FindRandomPoint (HomeLocation, RoamRadius)
    │   → Sets RandomPatrolPoint
    ├── BTTask_MoveTo (RandomPatrolPoint, AcceptanceRadius: 50)
    └── BTTask_Wait (Rand(3, 8))
```

---

## 4. BTTask_AttackTarget (Custom Blueprint Task)

### Parent: BTTask_BlueprintBase

### Event Receive Execute AI:
```
→ Cast AIController to ZombieAIController
→ Get Controlled Pawn → Cast to ZombieBase
→ Get Blackboard → Get TargetActor
→ If Zombie != null and Target != null:
    → Zombie.AttackTarget(Target)
    → Finish Execute: Success
→ Else:
    → Finish Execute: Failed
```

---

## 5. BTTask_FindRandomPoint (Custom Blueprint Task)

### Event Receive Execute AI:
```
→ Get AIController
→ Get Blackboard → Get HomeLocation
→ Get RoamRadius from Blackboard
→ Use "Get Random Reachable Point in Radius" (NavMesh)
    → Origin: HomeLocation
    → Radius: RoamRadius
    → Return: RandomPoint
→ If found:
    → Set Blackboard "RandomPatrolPoint" = RandomPoint
    → Finish Execute: Success
→ Else:
    → Finish Execute: Failed
```

---

## 6. BP_ZombieSpawner (Blueprint z C++ klasy ZombieSpawner)

### Components:
```
Root: SceneComponent
└── BoxComponent (SpawnVolume)
    └── Size: 1000x1000x500
```

### Variables:
| Name | Type | Default |
|------|------|---------|
| ZombieClasses | Array<TSubclassOf<ZombieBase>> | [BP_Zombie_Walker] |
| MaxZombies | int32 | 10 |
| SpawnInterval | float | 30.0 |
| bOnlyNight | bool | false |

### Event Graph:

**BeginPlay**
```
→ Super::BeginPlay()
→ Start SpawnTimer (Loop, Interval: SpawnInterval)
```

**SpawnTimer**
```
→ If DayNightManager.IsNight() or !bOnlyNight:
    → If CurrentZombieCount < MaxZombies:
        → RandomLocation = FindRandomPointInVolume()
        → Adjust Z to ground (LineTraceDown)
        → ZombieClass = Random from ZombieClasses
        → Spawn ZombieClass at RandomLocation
        → Add to SpawnedZombies array
```

---

## 7. BP_BuildingPiece_Foundation (Blueprint z BuildingPiece)

### Components:
```
Root: StaticMeshComponent
├── Mesh: SM_Foundation_Wood (or placeholder cube)
├── BoxCollision
└── DestructionMesh (optional — for fracture)
```

### Variables:
| Name | Type | Default |
|------|------|---------|
| BuildingTier | EBuildingTier | Wood |
| PlacementGhost | Material | M_Ghost_Green |
| PlacementInvalid | Material | M_Ghost_Red |

### Event Graph:

**OnRep_Health** (from C++)
```
→ HealthPercent = CurrentHealth / MaxHealth
→ If HealthPercent > 0.75:
    → SetMaterial (Undamaged)
→ Else If HealthPercent > 0.5:
    → SetMaterial (SlightlyDamaged) // cracks
→ Else If HealthPercent > 0.25:
    → SetMaterial (Damaged) // more cracks
→ Else:
    → SetMaterial (HeavilyDamaged) // burning/smoking)
```

**DestroyBuilding** (Override)
```
→ Spawn Emitter: P_BuildingDestroy
→ Spawn Sound: S_BuildingCollapse
→ Spawn Debris meshes (physics)
→ Drop resources (50% of cost)
→ Super::DestroyBuilding()
```

---

## 8. BP_ClaimBlock (Blueprint z ClaimBlock)

### Components:
```
Root: StaticMeshComponent
├── Mesh: SM_ClaimBlock
├── SphereComponent (Radius: 3000)
└── DecalComponent (Radius: 3000, Material: M_ClaimRadius)
```

### Event Graph:

**BeginPlay**
```
→ Super::BeginPlay()
→ If OwnerPlayerState == GetLocalPlayer():
    → Decal.SetVisibility(true) // show radius to owner
    → Decal.SetColor(Green)
→ Else:
    → Decal.SetVisibility(false)
```

**OnRep_Health**
```
→ Update material color based on health
→ If Health < 20%:
    → Play particle: P_Sparks
    → Play sound: S_BeepWarning (loop)
```

---

## 9. BP_WreckedVehicle_Buggy (Blueprint z WreckedVehicle)

### Components:
```
Root: StaticMeshComponent
├── Mesh: SM_WreckedBuggy
├── SkeletalMeshComponent (optional — for animated parts)
├── WidgetComponent (InteractionPrompt)
│   └── Widget: WBP_InteractionPrompt
└── ParticleSystemComponent
    └── Template: P_Smoke (subtle, from engine)
```

### Variables:
| Name | Type | Default |
|------|------|---------|
| RequiredParts | Array<FName> | ["Engine", "Wheels", "Battery"] |
| InstalledParts | Array<FName> | [] |
| InteractionText | String | "Hold E to Inspect" |

### Event Graph:

**OnInteract** (called by player)
```
→ If !bIsFullyAssembled:
    → Show WBP_VehicleAssembly UI
    → Pass RequiredParts and InstalledParts
→ Else:
    → EnterVehicle(Player)
```

**InstallPart** (called from UI)
```
→ If PartName in RequiredParts and PartName not in InstalledParts:
    → Add to InstalledParts
    → Play sound: S_MechanicWrench
    → Spawn particle: P_Sparks at part socket
    → Update AssemblyProgress
    → If All parts installed:
        → Play sound: S_EngineStart
        → Spawn particle: P_ExhaustSmoke
        → bIsFullyAssembled = true
        → Update InteractionText to "Hold E to Enter"
```

---

## 10. BP_Weapon_Pistol (Blueprint z WeaponBase)

### Components:
```
Root: SkeletalMeshComponent
├── Mesh: SK_Pistol
├── SceneComponent (MuzzleLocation)
│   └── Location: at barrel end
├── ParticleSystemComponent (MuzzleFlash)
│   └── Template: P_MuzzleFlash
│   └── AutoActivate: false
└── AudioComponent (FireSound)
    └── Sound: S_PistolFire
    └── AutoActivate: false
```

### Variables:
| Name | Type | Default |
|------|------|---------|
| WeaponName | String | "Pistol" |
| Damage | float | 25 |
| FireRate | float | 0.3 |
| MagazineSize | int32 | 12 |
| ReloadTime | float | 2.0 |
| RecoilPitch | float | -2.0 |
| RecoilYaw | float | 0.5 |
| AmmoType | ItemData | Ammo_9mm |

### Event Graph:

**Fire** (Override)
```
→ If CanFire():
    → CurrentAmmo--
    → Play AnimMontage: AM_PistolFire
    → MuzzleFlash.Activate()
    → FireSound.Play()
    → CameraShake: CS_PistolFire
    → LineTrace from MuzzleLocation
    → If Hit:
        → Spawn Decal: D_BulletHole at Hit.Location
        → Spawn Particle: P_Impact at Hit.Location
        → If HitActor is ZombieBase:
            → HitActor.TakeZombieDamage(Damage, Owner)
            → Show HitMarker on HUD
        → If HitActor is BuildingPiece:
            → HitActor.TakeBuildingDamage(Damage, Owner)
    → ApplyRecoil()
    → If CurrentAmmo <= 0:
        → Auto-reload after FireRate delay
```

**Reload**
```
→ If !bIsReloading and CurrentAmmo < MagazineSize:
    → bIsReloading = true
    → Play AnimMontage: AM_PistolReload
    → Play Sound: S_PistolReload
    → After ReloadTime:
        → CurrentAmmo = MagazineSize
        → bIsReloading = false
```

---

## 11. BP_LootContainer_Crate (Blueprint z LootContainer)

### Components:
```
Root: StaticMeshComponent
├── Mesh: SM_WoodenCrate
├── BoxCollision
└── WidgetComponent (InteractionPrompt)
```

### Event Graph:

**BeginPlay**
```
→ Super::BeginPlay()
→ Set Lid rotation to closed
```

**OnInteract**
```
→ If !bHasBeenLooted:
    → Open Lid (Rotate 90 degrees over 0.3s)
    → Play Sound: S_CrateOpen
    → Show WBP_LootContainer UI
    → Pass CurrentLoot array
→ Else:
    → Show "Empty" prompt
```

**OnLootTaken**
```
→ If CurrentLoot is empty:
    → bHasBeenLooted = true
    → Start RespawnTimer
```

---

## 12. BP_DayNightManager (Blueprint z DayNightManager)

### Components:
```
Root: SceneComponent
├── DirectionalLight (Sun)
├── SkyAtmosphere
├── ExponentialHeightFog
├── PostProcessVolume
└── SkyLight
```

### Event Graph:

**Tick**
```
→ Super::Tick()
→ Update Sun rotation
→ Update SkyAtmosphere
→ Update Fog density/color
→ Update PostProcess (eye adaptation)
→ If bIsNight != PreviousIsNight:
    → If bIsNight:
        → OnNightStarted.Broadcast()
        → Set FogColor to dark blue
        → Set SkyLight intensity to 0.1
    → Else:
        → OnDayStarted.Broadcast()
        → Set FogColor to light gray
        → Set SkyLight intensity to 1.0
```

---

## 13. Input Mapping Context (IMC_Default)

### Actions:
| Action Name | Key/Bind | Trigger |
|-------------|----------|---------|
| IA_Move | WASD | Axis (Value) |
| IA_Look | Mouse | Axis (Value) |
| IA_Jump | Space | Pressed |
| IA_Sprint | Left Shift | Pressed / Released |
| IA_Interact | E | Pressed |
| IA_Fire | Left Mouse | Pressed |
| IA_Aim | Right Mouse | Pressed / Released |
| IA_Reload | R | Pressed |
| IA_Inventory | Tab | Pressed |
| IA_Crafting | C | Pressed |
| IA_BuildMode | B | Pressed |
| IA_Map | M | Pressed |
| IA_Crouch | Left Ctrl | Pressed / Released |
| IA_Prone | Z | Pressed / Released |
| IA_VoiceChat | V | Pressed / Released |
| IA_PushToTalk | T | Pressed / Released |
| IA_EmoteMenu | G | Pressed |
| IA_Slot1 | 1 | Pressed |
| IA_Slot2 | 2 | Pressed |
| IA_Slot3 | 3 | Pressed |
| IA_Slot4 | 4 | Pressed |
| IA_Slot5 | 5 | Pressed |
| IA_Slot6 | 6 | Pressed |

---

## 14. WBP_InteractionPrompt (Widget Blueprint)

### Widget Hierarchy:
```
CanvasPanel
└── Border — Background: #000000, Opacity: 0.7, Padding: 8
    └── HorizontalBox
        ├── TextBlock (KeyText) — "[E]", FontSize: 14, Bold, White
        └── TextBlock (ActionText) — "Open", FontSize: 14, White
```

### Usage:
- Attach as WidgetComponent to interactable actors
- Face camera (SetWorldRotation to face player)
- Show/hide based on player distance and line of sight

---

## 15. BP_Projectile_Rocket (Blueprint Actor)

### Components:
```
Root: ProjectileMovementComponent
├── StaticMeshComponent (RocketMesh)
├── ParticleSystemComponent (Trail)
└── SphereCollision (ExplosionRadius: 300)
```

### Event Graph:

**OnHit**
```
→ Spawn Emitter: P_Explosion at Hit.Location
→ Play Sound: S_Explosion
→ CameraShake: CS_Explosion
→ Apply radial damage:
    → Radius: 300
    → Damage: 300
    → DamageType: Explosive
→ Destroy self
```
