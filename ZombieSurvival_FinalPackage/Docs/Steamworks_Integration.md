# STEAMWORKS INTEGRATION

## Wymagania
- Konto Steamworks (https://partner.steamgames.com/)
- Opłata rejestracyjna: $100 USD (jednorazowa)
- App ID (przydzielone przez Valve po rejestracji)
- Steam SDK (pobierz z Steamworks)

---

## Krok 1: Włącz Steam Subsystem

### 1.1 DefaultEngine.ini
Dodaj do `Config/DefaultEngine.ini`:

```ini
[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="/Script/OnlineSubsystemUtils.IpNetDriver")

[OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=true
SteamDevAppId=480  ; Użyj 480 dla testów (SpaceWar), potem zamień na swój AppID
bInitServerOnClient=true

[/Script/OnlineSubsystemSteam.SteamNetDriver]
NetConnectionClassName="/Script/OnlineSubsystemSteam.SteamNetConnection"
```

### 1.2 DefaultGame.ini
```ini
[/Script/Engine.GameSession]
MaxPlayers=50
```

---

## Krok 2: Kod C++ - Steam Session

Dodaj do `SurvivalGameInstance.h`:

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SurvivalGameInstance.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API USurvivalGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Steam")
    void HostSession(int32 MaxPlayers, bool bIsLAN);

    UFUNCTION(BlueprintCallable, Category = "Steam")
    void FindSessions();

    UFUNCTION(BlueprintCallable, Category = "Steam")
    void JoinSession(int32 SessionIndex);

    UFUNCTION(BlueprintCallable, Category = "Steam")
    void DestroySession();

    UFUNCTION(BlueprintCallable, Category = "Steam")
    void ShowSteamInviteOverlay();

    UFUNCTION(BlueprintPure, Category = "Steam")
    FString GetSteamPlayerName() const;

    UFUNCTION(BlueprintPure, Category = "Steam")
    bool IsSteamInitialized() const;

private:
    FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
    FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
    FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
    FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

    TSharedPtr<class FOnlineSessionSearch> SessionSearch;

    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
```

---

## Krok 3: Achievements

Utwórz `SteamAchievementManager.h`:

```cpp
UCLASS()
class ASteamAchievementManager : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void UnlockAchievement(const FString& AchievementID);

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void SetStat(const FString& StatName, int32 Value);

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    int32 GetStat(const FString& StatName) const;

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void IncrementStat(const FString& StatName, int32 Amount = 1);
};
```

---

## Krok 4: Steam Cloud

W `DefaultEngine.ini`:
```ini
[OnlineSubsystemSteam]
bEnabled=true
SteamDevAppId=YOUR_APP_ID
bUseSteamCloud=true

[/Script/Engine.GameEngine]
+SteamCloudEnabled=true
```

---

## Krok 5: Build & Upload

### 5.1 Packaging
1. **File → Package Project → Windows**
2. Ustaw konfigurację: **Shipping**
3. Wybierz folder docelowy

### 5.2 Steam Upload (SteamPipe)
1. Pobierz **Steamworks SDK**
2. Utwórz `app_build_YourAppID.vdf`:

```vdf
"AppBuild"
{
    "AppID" "YOUR_APP_ID"
    "Desc" "Build v0.1"
    "ContentRoot" "C:\Path\To\Packaged\Game"
    "BuildOutput" "C:\Path\To\BuildOutput"
    "SetLive" ""

    "Depots"
    {
        "YOUR_DEPOT_ID"
        {
            "FileMapping"
            {
                "LocalPath" "*"
                "DepotPath" "."
                "recursive" "1"
            }
        }
    }
}
```

3. Uruchom: `steamcmd.exe +login your_steam_username +run_app_build ..\scripts\app_build_YourAppID.vdf +quit`

---

## Krok 6: Steam Store Page

### Wymagane elementy:
- **Capsule images** (231x87, 467x181, 616x353, 460x215)
- **Screenshots** (minimum 5, 1920x1080)
- **Trailer** (opcjonalnie ale zalecane)
- **Opis gry** (krótki i długi)
- **System requirements**
- **Cena** (sugerowane: 15-25 EUR dla EA)
- **Tagi**: Survival, Zombie, Open World, Crafting, Multiplayer, Base Building

---

## Krok 7: Steam Deck Compatibility

1. W **Steamworks → Edit Steamworks Settings → Steam Deck**
2. Zaznacz: **Runs on Steam Deck**
3. Dodaj kontroler glyphs
4. Testuj w **Proton** (SteamOS)

---

## Anti-Cheat (zalecane)

### Easy Anti-Cheat (EAC)
- Darmowy dla gier na Steam
- Integracja z UE5: https://dev.epicgames.com/docs/game-services/anti-cheat

### VAC (Valve Anti-Cheat)
- Automatyczny jeśli używasz Steamworks
- Wymaga dedicated server

---

## Checklist przed wydaniem na Steam

- [ ] Gra się kompiluje w Shipping
- [ ] Multiplayer działa (testuj z friendami)
- [ ] Steam Overlay działa (Shift+Tab)
- [ ] Achievements są zdefiniowane w Steamworks
- [ ] Cloud saves działają
- [ ] Steam Deck kompatybilność przetestowana
- [ ] Store page jest kompletna
- [ ] Trailer/screenshots są gotowe
- [ ] Cena jest ustalona
- [ ] EULA / Privacy Policy dodane
- [ ] Build został przesłany na Steam (branches: default, beta)
