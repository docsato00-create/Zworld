# ANIMATION BLUEPRINT SETUP

## Required Animations (Mixamo + retarget to UE5 Mannequin)

### Player Character
| Animation | Mixamo Name | Usage |
|-----------|-------------|-------|
| Idle | "Idle" | Default standing |
| Walk | "Walking" | Forward movement |
| Run | "Running" | Sprint |
| Jump_Start | "Jumping" | Jump begin |
| Jump_Loop | "Falling Idle" | In air |
| Jump_Land | "Landing" | Landing |
| Melee_Swing | "Sword And Shield Slash" | Melee attack |
| Reload | "Rifle Reload" | Weapon reload |
| Death | "Dying" | Player death |

### Zombie Character
| Animation | Mixamo Name | Usage |
|-----------|-------------|-------|
| Zombie_Idle | "Zombie Idle" | Default |
| Zombie_Walk | "Zombie Walk" | Day movement |
| Zombie_Run | "Zombie Run" | Night/chase |
| Zombie_Attack | "Zombie Attack" | Attack |
| Zombie_Death | "Zombie Dying" | Death |

## AnimBlueprint Setup

### ABP_Player
```
State Machine: Locomotion
├── Idle (Speed < 10)
├── Walk (Speed 10-300)
├── Run (Speed 300-600)
├── Jump_Start (IsFalling && Velocity.Z > 0)
├── Jump_Loop (IsFalling && Velocity.Z <= 0)
└── Jump_Land (!IsFalling, triggered on land)

State Machine: Combat
├── Unarmed
├── Melee (IsMeleeAttacking)
├── Rifle_Idle
├── Rifle_Aim (bIsAiming)
├── Rifle_Fire (IsFiring)
└── Reload (bIsReloading)

Blend Spaces:
- BS_Locomotion: Direction (-180 to 180) x Speed (0 to 600)
- BS_Combat: Direction x Speed (0 to 300)
```

### ABP_Zombie
```
State Machine: ZombieLocomotion
├── Idle (Speed < 10)
├── Walk (Speed 10-200)
├── Run (Speed 200-400)
├── Attack (IsAttacking)
└── Death (IsDead)

Blend Space:
- BS_ZombieWalk: Speed (0 to 400)
```

## IK Setup
- **Foot IK**: Adjust foot height to terrain
- **Aim Offset**: Look up/down, left/right for aiming
- **Hand IK**: Attach weapon to hand socket

## Retargeting (UE5)
1. Import Mixamo FBX
2. Use "IK Rig" for source (Mixamo skeleton)
3. Use "IK Rig" for target (UE5 Mannequin)
4. Create "IK Retargeter"
5. Export retargeted animations

## Animation Montages
- MT_MeleeAttack: Section "Swing", "Hit", "Recover"
- MT_Reload: Section "Start", "MagOut", "MagIn", "End"
- MT_Death: Section "Death", "Dead" (loop)
- MT_HitReaction: Section "Hit", "Recover"
