# ARCHITEKTURA PROJEKTU UE5
## Moduły i Struktura Folderów

```
ZombieSurvival/
├── Source/
│   ├── ZombieSurvival/
│   │   ├── Core/                    # Klasy bazowe, enums, structs
│   │   ├── Player/                  # Character, Controller, PlayerState
│   │   ├── Survival/                # Hunger, thirst, health, stamina
│   │   ├── Inventory/               # Item definitions, container, crafting
│   │   ├── Building/                # Building pieces, claim block, decay
│   │   ├── Vehicles/                # Wrecked vehicle, assembly, driving
│   │   ├── AI/                      # Zombie base, behavior tree tasks/services
│   │   ├── Environment/             # Day/Night, weather, biomes
│   │   ├── Weapons/                 # Firearms, melee, attachments
│   │   └── Multiplayer/             # GameMode, GameState, replication helpers
│   └── ZombieSurvivalEditor/
├── Content/
│   ├── Blueprints/
│   │   ├── AI/                      # Behavior Trees, Blackboards
│   │   ├── Building/                # BP_BuildingPiece_*
│   │   ├── Items/                   # BP_Item_*, DT_Items
│   │   ├── UI/                      # WBP_Inventory, WBP_Crafting, WBP_HUD
│   │   ├── Vehicles/                # BP_Wrecked_*, BP_Assembled_*
│   │   └── Weapons/                 # BP_Weapon_*
│   ├── Maps/
│   │   ├── Dev_TestMap
│   │   └── MainMenu
│   └── Assets/                      # Darmowe assety z Marketplace/Quixel
└── Config/
```

## Kluczowe Klasy C++

### Core
- `USurvivalGameInstance` — globalne dane, session management
- `ASurvivalGameMode` — spawn, respawn, rules, raid windows
- `ASurvivalGameState` — replicated time of day, weather, horde state
- `ASurvivalPlayerState` — level, XP, skill points, reputation
- `ASurvivalPlayerController` — input, UI management, server RPCs

### Player
- `ASurvivalCharacter` — movement, interaction, combat
- `USurvivalComponent` — hunger, thirst, health, stamina, temperature
- `UInventoryComponent` — item container, weight, quickbar
- `UBuildingComponent` — placement preview, snapping, rotation

### AI
- `AZombieBase` — base character, stats, type
- `UHordeManagerComponent` — zarządzanie spawnem i ruchem hord
- `AZombieSpawner` — volume-based spawning, day/night modifiers

### Building
- `ABuildingPiece` — base class for all structures
- `AClaimBlock` — territory ownership, raid protection
- `UBuildingManager` — grid validation, overlap checks

### Vehicles
- `AWreckedVehicle` — interactable wreck, part slots
- `AAssembledVehicle` — drivable pawn, fuel consumption
- `UVehiclePartData` — struct: tier, condition, stats

### Environment
- `ADayNightManager` — sun rotation, sky atmosphere, time curve
- `UBiomeVolume` — post-process, spawn tables, hazards
