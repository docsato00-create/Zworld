# SETUP PROJEKTU UE5 - Zombie Survival

## Wymagania
- Unreal Engine 5.4+ (zalecane 5.5)
- Visual Studio 2022 z workload "Game development with C++"
- Windows 10/11 64-bit
- 16GB+ RAM (32GB zalecane)
- SSD (kompilacja shaderów)

---

## Krok 1: Utwórz projekt w UE5

1. Otwórz Epic Games Launcher → Unreal Engine
2. Kliknij **Create Project**
3. Wybierz szablon: **Games → Blank**
4. Ustawienia:
   - **Blueprint or C++?** → C++
   - **Starter Content** → No (dodamy własne)
   - **Raytracing** → Off (dla wydajności)
   - **Target Platform** → Desktop
5. Nazwa projektu: `ZombieSurvival`
6. Kliknij **Create**

---

## Krok 2: Skopiuj pliki źródłowe

Skopiuj wszystkie pliki z folderu `Source/` tego pakietu do:
```
ZombieSurvival/Source/ZombieSurvival/
```

Struktura powinna wyglądać tak:
```
Source/
└── ZombieSurvival/
    ├── Core/
    │   └── SurvivalTypes.h
    ├── Player/
    │   ├── SurvivalCharacter.h/.cpp
    │   ├── SurvivalPlayerController.h/.cpp
    │   ├── SurvivalPlayerState.h/.cpp
    │   ├── SurvivalComponent.h/.cpp
    │   ├── InventoryComponent.h/.cpp
    │   └── BuildingComponent.h/.cpp
    ├── AI/
    │   ├── ZombieBase.h/.cpp
    │   ├── HordeManager.h/.cpp
    │   └── ZombieSpawner.h/.cpp
    ├── Building/
    │   ├── ClaimBlock.h/.cpp
    │   ├── BuildingPiece.h/.cpp
    │   └── BuildingComponent.h/.cpp (już w Player/)
    ├── Vehicles/
    │   ├── WreckedVehicle.h/.cpp
    │   └── AssembledVehicle.h/.cpp
    ├── Inventory/
    │   ├── ItemData.h
    │   ├── LootContainer.h/.cpp
    │   ├── CraftingRecipe.h/.cpp
    │   └── CraftingStation.h/.cpp
    ├── Weapons/
    │   └── WeaponBase.h/.cpp
    ├── Environment/
    │   └── DayNightManager.h/.cpp
    └── Multiplayer/
        └── SurvivalGameMode.h/.cpp
```

---

## Krok 3: Zaktualizuj Build.cs

Otwórz `Source/ZombieSurvival/ZombieSurvival.Build.cs` i upewnij się, że zawiera:

```csharp
using UnrealBuildTool;

public class ZombieSurvival : ModuleRules
{
    public ZombieSurvival(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "NavigationSystem",
            "GameplayTasks",
            "AIModule",
            "UMG",
            "OnlineSubsystem",
            "OnlineSubsystemSteam"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Projects"
        });
    }
}
```

---

## Krok 4: Skompiluj projekt

1. W UE5 Editor: **Tools → Refresh Visual Studio Project**
2. Zamknij UE5
3. Otwórz `.sln` w Visual Studio 2022
4. Ustaw konfigurację: **Development Editor | Win64**
5. Kliknij **Build → Build Solution** (Ctrl+Shift+B)
6. Po kompilacji uruchom projekt z VS: **Debug → Start Without Debugging** (Ctrl+F5)

---

## Krok 5: Konfiguracja w Editorze

### 5.1 Game Mode
1. Otwórz `Edit → Project Settings → Maps & Modes`
2. **Default GameMode** → `BP_SurvivalGameMode` (utwórz BP z C++ klasy)
3. **Default Pawn Class** → `BP_SurvivalCharacter`
4. **Player Controller Class** → `BP_SurvivalPlayerController`

### 5.2 Input (Enhanced Input)
1. Utwórz folder `Content/Input/`
2. Utwórz **Input Mapping Context** → `IMC_Default`
3. Dodaj akcje:
   - IA_Move (Axis2D: WASD)
   - IA_Look (Axis2D: Mouse)
   - IA_Jump (Action: Space)
   - IA_Sprint (Action: Left Shift)
   - IA_Interact (Action: E)
   - IA_Fire (Action: Left Mouse)
   - IA_Aim (Action: Right Mouse)
   - IA_BuildMode (Action: B)
   - IA_Inventory (Action: Tab)
   - IA_Reload (Action: R)

### 5.3 Day Night Manager
1. Utwórz BP z `DayNightManager`
2. Ustaw **SunLight** (DirectionalLight z mapy)
3. Dodaj do mapy

### 5.4 Zombie Spawner
1. Utwórz BP z `ZombieSpawner`
2. Dodaj do mapy jako Volume
3. Ustaw `ZombieTypes` (utwórz BP z `ZombieBase`)

---

## Krok 6: Darmowe assety (opcjonalnie)

Zobacz `FreeAssets.md` dla listy darmowych assetów z UE Marketplace.

---

## Krok 7: Pierwszy test

1. Uruchom PIE (Play In Editor)
2. Sprawdź:
   - Movement (WASD)
   - Sprint (Shift) + stamina drain
   - Hunger/Thirst decay w czasie
   - Day/Night cycle
   - Zombie spawn w spawner volume
   - Building mode (B)
   - Claim block placement

---

## Rozwiązywanie problemów

| Problem | Rozwiązanie |
|---------|------------|
| "Cannot open include file" | Sprawdź ścieżki w .h, upewnij się że module name jest poprawny |
| "Unresolved external symbol" | Dodaj brakujący .cpp do projektu lub sprawdź Build.cs |
| Zombie nie spawnują się | Sprawdź czy NavMesh jest zbudowany (P → Show → Navigation) |
| Multiplayer nie działa | Ustaw Net Mode na "Play As Client" w Multiplayer Options |
| UI się nie pokazuje | Upewnij się że Widgets są dodane do Viewport w PlayerController |

---

## Następne kroki

1. Stwórz Blueprinty z C++ klas
2. Dodaj assety 3D (modele zombie, broni, budynków)
3. Stwórz UI Widgets (HUD, Inventory, Crafting)
4. Skonfiguruj NavMesh dla AI
5. Dodaj dźwięki i efekty cząsteczkowe
6. Testuj multiplayer (2+ clients)
