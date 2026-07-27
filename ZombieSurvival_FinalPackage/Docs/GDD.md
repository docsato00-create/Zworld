# ZOMBIE SURVIVAL - GAME DESIGN DOCUMENT
## Wersja: Pre-Alpha 0.1 | Silnik: Unreal Engine 5.4+

---

## 1. ELEVATOR PITCH
Sandbox survival 3D z zombie w stylu low-poly / stylizowanym (Hurtworld). Gracz buduje bazy, składa pojazdy z wraków, walczy z hordami i innymi graczami. Noc przynosi śmierć — zombie stają się szybsze, agresywniejsze i organizują się w hordy.

## 2. CORE LOOPS

### 2.1 Main Loop (Sesja 1-3h)
1. **Spawn** → lootowanie okolicy → zdobycie jedzenia/wody
2. **Crafting podstawowy** → narzędzia, broń biała
3. **Budowa schronienia** → claim block → rozbudowa
4. **Eksploracja** → wraki pojazdów → składanie transportu
5. **Raid / Defence** → PvP lub horde nocna

### 2.2 Meta Loop (Długoterminowy)
- Zdobywanie blueprintów rzadkich części
- Ulepszanie claim block (poziomy 1-5)
- Bossy dropiące unikalne komponenty
- Map wipe co X tygodni (opcjonalnie)

---

## 3. MECHANIKI KLUCZOWE

### 3.1 Survival Needs
| Potrzeba | Spadek | Konsekwencja braku |
|----------|--------|-------------------|
| Głód | 1 pkt/min | Spadek HP, nie można sprintować |
| Pragnienie | 1.5 pkt/min | Szybszy spadek HP, vision blur |
| Stamina | dynamiczny | Brak sprintu, słabszy melee |
| Health | - | Śmierć, respawn z penalty |
| Radiation (opcjonalnie) | w strefach | Spadek max HP |

### 3.2 Dzień i Noc
- Cykl: 45 min real time = 24h in-game
- Dzień (6:00-20:00): Zombie wolne, rozproszone, mało agresywne
- Noc (20:00-6:00): Zombie +50% speed, +100% damage, hordy aktywne
- Pełnia: specjalne eventy hord

### 3.3 Zombie AI
- **Daytime**: Wander, Slow Chase (słyszą hałas w promieniu 10m)
- **Nighttime**: Fast Chase, Horde Coordination (grupowanie się)
- **Horde Mode**: Grupa 15-50 zombie przemieszcza się po mapie w kierunku gracza/bazy
- **Bossy**: Unikalne modele, mechaniki (np. spawner minionów, charge attack)

### 3.4 Budowanie Bazy
- Grid snapping 0.5m (foundation, wall, door, ceiling, roof)
- **Claim Block**: Sześcian 2x2x2m, wyznacza sferę własności (radius 30m)
  - W obrębie claim: tylko właściciel może budować/niszczyć
  - Claim Block ma HP (5000) i może być raidowany
  - Bez claim block: struktury mają 10x mniej HP
- **Decay**: Jeśli gracz nie zaloguje się przez 48h, struktury tracą HP
- **Raiding**: C4, satchel charges, melee (drewno), rockets (metal)

### 3.5 Pojazdy (Hurtworld-style)
- Na mapie rozrzucone **wraki** (5 typów: buggy, truck, bike, heli, boat)
- Wrak = rama + 3-5 slotów na części
- Części: Engine, Wheels, Battery, Fuel Tank, Transmission
- Części mają **tier** (scrap, standard, performance, racing)
- Składanie: przyłóż część do wraku → mini-game montażu (opcjonalnie)
- Pojazd zużywa paliwo, może być zniszczony, naprawiany kitami

### 3.6 Ekwipunek & Crafting
- Sloty: 24 inventory + 6 hotbar + 4 armor
- Weight system (opcjonalnie)
- Crafting stacje: Campfire, Workbench, Forge, Vehicle Lift, Research Table
- Blueprint system: znajdź fragment → połącz w pełny BP → możesz craftować

### 3.7 Broń
- Melee: Rock, Axe, Pickaxe, Sword, Baseball Bat
- Ranged: Bow, Pistol, Shotgun, Rifle, Sniper, Rocket Launcher
- Attachmenty: Scope, Silencer, Extended Mag, Laser
- Durability: wszystko się psuje

---

## 4. MAPA & EKSPLORACJA

### 4.1 Biomy
| Biom | Zagrożenia | Loot |
|------|-----------|------|
| Meadows (spawn) | Niskie | Podstawowy, drewno, kamień |
| Forest | Średnie | Więcej drewna, zwierzęta |
| Desert | Niskie (mało zombie), pragnienie x2 | Ruda metalu, wraki |
| Snow | Wysokie, mróz | High-tier loot, bossy |
| City | Ekstremalne (hordy) | Najlepszy loot, wraki |
| Radiation Zone | Ekstremalne | Endgame loot |

### 4.2 Punkty Zainteresowania
- Miasto (ruiny) — wysoki loot, hordy
- Stacja benzynowa — paliwo, części
- Baza wojskowa — broń, armor
- Jaskinie — ruda, bossy
- Wieża radiowa — high ground, elektronika

---

## 5. MULTIPLAYER

### 5.1 Tryby
- **PvE**: Gracze nie mogą się ranić, bazy raidują tylko zombie
- **PvP**: Full damage, raiding włączony
- **Softcore PvP**: PvP włączone, ale raiding tylko w godzinach wieczornych

### 5.2 Serwer
- Dedicated server (Linux/Windows)
- Max graczy: 50-100
- Map size: 4km x 4km (MVP), 8km x 8km (EA)
- Wipe schedule: co 2 tygodnie (MVP), monthly (EA)

---

## 6. PROGRESSION

### 6.1 Levels & XP
- Poziomy 1-50
- XP za: zabijanie, craftowanie, budowanie, eksplorację
- Punkty umiejętności: Strength, Agility, Endurance, Intellect, Perception

### 6.2 Tech Tree (w Research Table)
- Tier 0: Podstawowe narzędzia
- Tier 1: Broń palna, metalowe struktury
- Tier 2: Elektronika, pojazdy, C4
- Tier 3: Endgame armor, rockets, auto-turrety

---

## 7. MONETYZACJA (Steam)
- Płatna gra (15-25 EUR)
- Zero mikrotransakcji P2W
- Opcjonalne DLC kosmetyczne (skiny do broni, ubrań — po ugruntowaniu bazy graczy)

---

## 8. MILESTONES

| Faza | Czas | Co musi działać |
|------|------|----------------|
| MVP | 3 mies. | Movement, shooting, 1 zombie type, day/night, basic building, hunger/thirst |
| Alpha | 6 mies. | Multiplayer, vehicles, claim block, 3 biomes, 5 weapons |
| Closed Beta | 9 mies. | Horde system, boss, full crafting, PvP raiding |
| Steam EA | 12 mies. | Polishing, 4 biomes, Steamworks, anti-cheat |
| Full Release | 18-24 mies. | Content complete, marketing, community |
