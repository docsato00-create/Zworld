# DEDICATED SERVER & MULTIPLAYER SETUP

## 1. DEDICATED SERVER BUILD

### 1.1 Build Configuration

**File → Package Project → Build Configuration: Shipping Server**

Or command line:
```bash
# Windows Server
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun \
    -project="C:\Path\To\ZombieSurvival.uproject" \
    -noP4 \
    -platform=Win64 \
    -clientconfig=Shipping \
    -serverconfig=Shipping \
    -cook \
    -allmaps \
    -stage \
    -pak \
    -archive \
    -archivedirectory="C:\Builds\Server"

# Linux Server (cross-compile from Windows)
# Install Linux toolchain first
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun \
    -project="C:\Path\To\ZombieSurvival.uproject" \
    -noP4 \
    -platform=Linux \
    -clientconfig=Shipping \
    -serverconfig=Shipping \
    -cook \
    -server \
    -serverplatform=Linux \
    -stage \
    -pak \
    -archive \
    -archivedirectory="C:\Builds\LinuxServer"
```

### 1.2 Server Directory Structure
```
ZombieSurvivalServer/
├── ZombieSurvivalServer.exe (or ZombieSurvivalServer on Linux)
├── ZombieSurvival/
│   ├── Binaries/
│   ├── Content/
│   └── Plugins/
├── Engine/
└── ZombieSurvivalServer.bat (launch script)
```

### 1.3 Launch Scripts

**Windows: StartServer.bat**
```batch
@echo off
ZombieSurvivalServer.exe ZombieSurvival?listen?MaxPlayers=50 -server -log -port=7777
```

**Linux: start_server.sh**
```bash
#!/bin/bash
./ZombieSurvivalServer ZombieSurvival?listen?MaxPlayers=50 -server -log -port=7777
```

**With Config Overrides:**
```batch
ZombieSurvivalServer.exe ZombieSurvival?listen?MaxPlayers=50 -server -log -port=7777 -QueryPort=27015 -SteamServerName="My Zombie Server #1" -ServerPassword="secret123" -AdminPassword="admin123"
```

---

## 2. SERVER CONFIGURATION

### 2.1 Engine.ini (Server)
```ini
[/Script/OnlineSubsystemSteam.SteamNetDriver]
NetConnectionClassName="/Script/OnlineSubsystemSteam.SteamNetConnection"
ChannelLimit=65535

[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="/Script/OnlineSubsystemUtils.IpNetDriver")

[/Script/Engine.NetworkSettings]
+NetworkEmulationProfiles=(ProfileName="Bad",PacketLossPercentage=5.0,LatencyInMS=200,JitterInMS=50)
+NetworkEmulationProfiles=(ProfileName="Average",PacketLossPercentage=1.0,LatencyInMS=100,JitterInMS=20)
+NetworkEmulationProfiles=(ProfileName="Good",PacketLossPercentage=0.0,LatencyInMS=50,JitterInMS=5)

[/Script/OnlineSubsystemSteam.SteamNetworking]
P2PPacketLoss=0
P2PRelay=false
```

### 2.2 Game.ini (Server)
```ini
[/Script/Engine.GameSession]
MaxPlayers=50
MaxSpectators=10

[/Script/ZombieSurvival.SurvivalGameMode]
bEnablePvP=true
bEnableRaiding=true
RaidWindowStartHour=18.0
RaidWindowEndHour=6.0
RespawnDelay=10.0f

[/Script/ZombieSurvival.DayNightManager]
RealMinutesPerGameDay=45.0f

[/Script/ZombieSurvival.HordeManager]
MaxHordeSize=50
HordeSpawnInterval=300.0f

[/Script/ZombieSurvival.AntiCheatManager]
bEnableAntiCheat=true
MaxAllowedSpeed=1500.0f
bAutoKickEnabled=true
MaxInfractionsBeforeKick=5
```

### 2.3 DefaultGame.ini (Server)
```ini
[/Script/Engine.GameSession]
MaxPlayers=50

[/Script/ZombieSurvival.SurvivalGameMode]
+SpawnPoints=(X=0.0,Y=0.0,Z=100.0)
+SpawnPoints=(X=1000.0,Y=1000.0,Z=100.0)
+SpawnPoints=(X=-1000.0,Y=-1000.0,Z=100.0)
```

---

## 3. STEAM DEDICATED SERVER

### 3.1 SteamCMD Setup
```bash
# Download SteamCMD
# Windows: https://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip
# Linux: https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz

# Login anonymously
steamcmd.exe +login anonymous +app_update 1007 validate +quit

# 1007 = Steamworks SDK Redist (required for dedicated servers)
```

### 3.2 Server Registration
```cpp
// In SurvivalGameMode::InitGame
IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(FName("Steam"));
if (SteamSubsystem)
{
    IOnlineSessionPtr SessionInterface = SteamSubsystem->GetSessionInterface();
    if (SessionInterface.IsValid())
    {
        FOnlineSessionSettings SessionSettings;
        SessionSettings.bIsLANMatch = false;
        SessionSettings.bIsDedicated = true;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.NumPublicConnections = 50;
        SessionSettings.bUsesPresence = false;
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.bAllowInvites = false;
        SessionSettings.bUsesStats = true;

        SessionInterface->CreateSession(0, FName("ZombieSurvivalServer"), SessionSettings);
    }
}
```

---

## 4. SERVER ADMINISTRATION

### 4.1 RCON (Remote Console)
```cpp
// Simple TCP RCON server
UCLASS()
class URCONServer : public UObject
{
    UPROPERTY()
    FSocket* ListenSocket;

    UPROPERTY()
    FString RCONPassword;

    UFUNCTION()
    void StartRCON(int32 Port, const FString& Password);

    UFUNCTION()
    void ProcessCommand(const FString& Command, FSocket* Client);
};

// Commands via RCON:
// status - show player list
// kick <player> <reason>
// ban <player> <minutes> <reason>
// save - force save world
// say <message> - broadcast
// time <hour> - set time
// weather <type> - set weather
```

### 4.2 Server Logs
```cpp
// Automatic log rotation
void ASurvivalGameMode::WriteServerLog(const FString& Message)
{
    FString LogPath = FPaths::ProjectSavedDir() / TEXT("Logs") / TEXT("Server.log");
    FString Entry = FString::Printf(TEXT("[%s] %s\n"), *FDateTime::Now().ToString(), *Message);
    FFileHelper::SaveStringToFile(Entry, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}
```

### 4.3 Automated Restart
```bash
# restart_server.sh (Linux cron job every 6 hours)
#!/bin/bash
PID=$(pgrep -f ZombieSurvivalServer)
if [ -n "$PID" ]; then
    kill -SIGTERM $PID
    sleep 30
    if ps -p $PID > /dev/null; then
        kill -SIGKILL $PID
    fi
fi
./start_server.sh
```

---

## 5. MULTIPLAYER REPLICATION DEBUGGING

### 5.1 Replication Checklist
```cpp
// 1. Is actor set to replicate?
AActor::SetReplicates(true);

// 2. Are properties marked with Replicated/ReplicatedUsing?
UPROPERTY(ReplicatedUsing=OnRep_Health)
float Health;

// 3. Is GetLifetimeReplicatedProps implemented?
void AMyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMyActor, Health);
}

// 4. Is server authority checked?
if (GetLocalRole() == ROLE_Authority)
{
    // Server-only logic
}

// 5. Are RPCs marked correctly?
UFUNCTION(Server, Reliable)
void Server_DoSomething();

UFUNCTION(Client, Reliable)
void Client_DoSomething();

UFUNCTION(NetMulticast, Reliable)
void Multicast_DoSomething();
```

### 5.2 Common Replication Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| Client doesn't see changes | Missing DOREPLIFETIME | Add to GetLifetimeReplicatedProps |
| Server doesn't receive RPC | Not calling on client | Ensure function runs on owning client |
| Variables not updating | Missing ReplicatedUsing | Add OnRep_ function |
| Too much network traffic | No culling | Set NetCullDistanceSquared |
| Desync | Client prediction missing | Implement client-side prediction |
| Actor not spawning on clients | bReplicates=false | Set bReplicates=true |

### 5.3 Network Profiler
```
Console: netprofile enable
Play for 30 seconds
Console: netprofile disable
Open .nprof file in UnrealFrontend
```

---

## 6. LAG COMPENSATION

### 6.1 Server-Side Rewind
```cpp
// For hit detection, rewind player positions by ping/2
void AWeaponBase::Server_Fire_Implementation(FVector Origin, FVector Direction)
{
    float ClientPing = GetPlayerPing(); // ms
    float RewindTime = ClientPing / 2000.0f; // seconds

    // Get player positions at RewindTime ago
    for (APlayerState* PS : GetAllPlayerStates())
    {
        FVector HistoricalPosition = GetHistoricalPosition(PS, RewindTime);
        // Use HistoricalPosition for hit detection
    }
}
```

### 6.2 Input Buffering
```cpp
// Buffer inputs for 50ms to smooth out jitter
TArray<FPlayerInput> InputBuffer;

void ProcessBufferedInput()
{
    if (InputBuffer.Num() > 0)
    {
        FPlayerInput Input = InputBuffer[0];
        InputBuffer.RemoveAt(0);
        ApplyInput(Input);
    }
}
```

---

## 7. HOSTING PROVIDERS

### Recommended Server Hosting
| Provider | Price (50 slot) | Locations | Notes |
|----------|----------------|-----------|-------|
| OVHcloud | €50/month | EU, US, CA | Good DDoS protection |
| Hetzner | €40/month | EU, US | Cheap, reliable |
| Amazon G4dn | $80/month | Global | Flexible, scalable |
| Google Cloud | $90/month | Global | Good network |
| Pterodactyl | Free (self-host) | Any | Open source panel |

### Minimum Server Specs (50 players)
| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CPU | i7-9700K | Ryzen 7 5800X |
| RAM | 16GB DDR4 | 32GB DDR4 |
| Storage | 100GB SSD | 250GB NVMe |
| Network | 100Mbps | 1Gbps |
| OS | Windows Server 2019 / Ubuntu 22.04 |

---

## 8. SERVER MONITORING

### 8.1 Prometheus + Grafana Setup
```yaml
# docker-compose.yml
version: '3'
services:
  prometheus:
    image: prom/prometheus
    ports:
      - "9090:9090"
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml

  grafana:
    image: grafana/grafana
    ports:
      - "3000:3000"
```

### 8.2 Metrics to Track
- Player count over time
- Server FPS / tick rate
- Memory usage
- Network traffic (in/out)
- Zombie count
- Average player ping
- Error rate
- Save/load duration

---

## 9. MOD SUPPORT (WORKSHOP)

### 9.1 Modding API
```cpp
// Expose functions to Blueprint for modders
UFUNCTION(BlueprintCallable, Category="Modding")
void RegisterCustomItem(TSubclassOf<UItemData> ItemClass);

UFUNCTION(BlueprintCallable, Category="Modding")
void RegisterCustomRecipe(TSubclassOf<UCraftingRecipe> RecipeClass);

UFUNCTION(BlueprintCallable, Category="Modding")
void RegisterCustomZombie(TSubclassOf<AZombieBase> ZombieClass);
```

### 9.2 Steam Workshop Integration
```cpp
// In Build.cs
PublicDependencyModuleNames.Add("OnlineSubsystemSteam");

// Upload mod
SteamUGC()->CreateItem(AppId, k_EWorkshopFileTypeCommunity);
SteamUGC()->SetItemContent(UGCUpdateHandle, ModFolderPath);
SteamUGC()->SubmitItemUpdate(UGCUpdateHandle, "Update description");
```
