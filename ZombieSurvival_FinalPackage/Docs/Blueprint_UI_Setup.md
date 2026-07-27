# BLUEPRINT UI SETUP - KOMPLETNE INSTRUKCJE

## Wymagane Widget Blueprints (WBP_)

---

## 1. WBP_InventorySlot (Widget Blueprint)

### Parent Class: UserWidget

### Widget Hierarchy:
```
CanvasPanel (Root)
└── Border (SlotBorder)
    └── Overlay
        ├── Image (ItemIcon) — Size: 64x64
        ├── TextBlock (QuantityText) — Anchored: BottomRight
        ├── TextBlock (DurabilityText) — Anchored: BottomLeft, FontSize: 8
        └── Border (HoverOverlay) — Color: White, Opacity: 0.1, Visibility: Hidden
```

### Variables:
| Name | Type | Default | Description |
|------|------|---------|-------------|
| SlotIndex | int32 | 0 | Index w inventory |
| IsHotbarSlot | bool | false | Czy to hotbar |
| ItemData | UItemData (Soft Obj Ref) | None | Aktualny item |
| Quantity | int32 | 0 | Ilość |
| Condition | float | 100 | Stan (dla broni/części) |

### Functions:

**SetSlotData**
```
Inputs: Item (UItemData), Amount (int32), Cond (float), Index (int32)
→ Set ItemData = Item
→ Set Quantity = Amount
→ Set Condition = Cond
→ Set SlotIndex = Index
→ If Item != null:
    → ItemIcon.SetBrushFromTexture(Item.Icon)
    → ItemIcon.SetVisibility(Visible)
    → QuantityText.SetText(Amount > 1 ? Amount : "")
    → QuantityText.SetVisibility(Amount > 1 ? Visible : Hidden)
    → DurabilityText.SetText(Cond < 100 ? Floor(Cond) + "%" : "")
    → DurabilityText.SetVisibility(Cond < 100 ? Visible : Hidden)
→ Else:
    → ItemIcon.SetVisibility(Hidden)
    → QuantityText.SetVisibility(Hidden)
    → DurabilityText.SetVisibility(Hidden)
```

**OnMouseButtonDown** (Override)
```
→ If Left Click:
    → Cast to WBP_Inventory
    → Call OnSlotClicked(SlotIndex)
→ If Right Click:
    → Show ContextMenu
→ Return Handled
```

**OnMouseEnter** (Override)
```
→ HoverOverlay.SetVisibility(Visible)
→ If ItemData != null:
    → Create WBP_ItemTooltip
    → Set tooltip data
    → Add to viewport at mouse position
```

**OnMouseLeave** (Override)
```
→ HoverOverlay.SetVisibility(Hidden)
→ Remove tooltip from viewport
```

**OnDragDetected** (Override)
```
→ If ItemData != null:
    → Create DragDropOperation (class: UDragDropOperation)
    → Set Payload = SlotIndex
    → Set DefaultDragVisual = self (or icon image)
    → Return DragDropOperation
```

**OnDrop** (Override)
```
→ Cast Operation.Payload to int32 (DraggedSlotIndex)
→ Cast to WBP_Inventory
→ Call OnSlotDropped(DraggedSlotIndex, SlotIndex)
```

---

## 2. WBP_Inventory (Widget Blueprint)

### Parent Class: InventoryWidget (C++)

### Widget Hierarchy:
```
CanvasPanel (Root)
├── Border (Background) — Color: #1a1a1a, Opacity: 0.95
│   └── Overlay
│       ├── VerticalBox (MainLayout)
│       │   ├── HorizontalBox (Header)
│       │   │   ├── TextBlock (Title) — "INVENTORY", FontSize: 24, Bold
│       │   │   ├── Spacer
│       │   │   └── TextBlock (WeightText) — "0.0 / 100.0 kg"
│       │   ├── UniformGridPanel (InventoryGrid) — Columns: 6, Rows: 4
│       │   ├── Separator (Spacer: 20px)
│       │   ├── TextBlock (HotbarLabel) — "HOTBAR"
│       │   └── UniformGridPanel (HotbarGrid) — Columns: 6, Rows: 1
│       ├── Button (CloseButton) — Anchored: TopRight, Size: 30x30
│       │   └── TextBlock — "X"
│       └── Border (ItemDetailsPanel) — Anchored: Right, Width: 250
│           └── VerticalBox
│               ├── Image (DetailsIcon) — Size: 128x128
│               ├── TextBlock (DetailsName) — FontSize: 18, Bold
│               ├── TextBlock (DetailsDescription) — WrapText: true
│               ├── TextBlock (DetailsStats) — "Damage: 25\nRange: 2000"
│               └── VerticalBox (ActionButtons)
│                   ├── Button (UseButton) — "USE"
│                   ├── Button (DropButton) — "DROP"
│                   └── Button (SplitButton) — "SPLIT"
```

### Event Graph:

**OnInventoryRefreshed** (Event from C++)
```
→ Clear InventoryGrid
→ For i = 0 to PlayerInventory.Slots.Length - 1:
    → Create WBP_InventorySlot
    → SetSlotData(PlayerInventory.Slots[i].Item, 
                  PlayerInventory.Slots[i].Quantity,
                  PlayerInventory.Slots[i].Condition, i)
    → Add to InventoryGrid at (i / 6, i % 6)
→ Clear HotbarGrid
→ For i = 0 to 5:
    → Create WBP_InventorySlot
    → If i < PlayerInventory.Slots.Length:
        → SetSlotData(PlayerInventory.Slots[i].Item, ...)
    → Set IsHotbarSlot = true
    → Add to HotbarGrid at (0, i)
```

**OnSlotClicked**
```
→ If Shift pressed:
    → SplitStack(SlotIndex)
→ Else:
    → UseItem(SlotIndex)
```

**OnSlotRightClicked**
```
→ Show ContextMenu at mouse position:
    ├── "Use" → UseItem(SlotIndex)
    ├── "Drop" → DropItemToWorld(SlotIndex)
    ├── "Drop Half" → DropItemToWorld(SlotIndex, Quantity/2)
    ├── "Equip" → EquipToHotbar(SlotIndex)
    └── "Examine" → ShowDetails(SlotIndex)
```

**ShowDetails**
```
→ Get ItemData from slot
→ DetailsIcon.SetBrushFromTexture(ItemData.Icon)
→ DetailsName.SetText(ItemData.DisplayName)
→ DetailsDescription.SetText(ItemData.Description)
→ Build stats string based on item type:
    → If weapon: "Damage: X\nRange: Y\nFire Rate: Z"
    → If consumable: "Hunger: +X\nThirst: +Y\nHealth: +Z"
    → If building: "Health: X\nTier: Y"
```

**CloseButton.OnClicked**
```
→ ToggleInventory() (z PlayerController)
```

---

## 3. WBP_HUD (Widget Blueprint)

### Parent Class: SurvivalHUDWidget (C++)

### Widget Hierarchy:
```
CanvasPanel (Root)
├── VerticalBox (LeftPanel) — Anchored: BottomLeft, Margin: 20
│   ├── HorizontalBox (HealthRow)
│   │   ├── Image (HealthIcon) — Size: 24x24, Texture: heart_icon
│   │   └── ProgressBar (HealthBar) — Size: 200x20
│   │       └── Border (Fill) — Color: #e74c3c
│   ├── HorizontalBox (HungerRow)
│   │   ├── Image (HungerIcon) — Size: 24x24, Texture: food_icon
│   │   └── ProgressBar (HungerBar) — Size: 200x20
│   │       └── Border (Fill) — Color: #e67e22
│   ├── HorizontalBox (ThirstRow)
│   │   ├── Image (ThirstIcon) — Size: 24x24, Texture: water_icon
│   │   └── ProgressBar (ThirstBar) — Size: 200x20
│   │       └── Border (Fill) — Color: #3498db
│   └── HorizontalBox (StaminaRow)
│       ├── Image (StaminaIcon) — Size: 24x24, Texture: bolt_icon
│       └── ProgressBar (StaminaBar) — Size: 200x20
│           └── Border (Fill) — Color: #2ecc71
│
├── HorizontalBox (BottomCenter) — Anchored: BottomCenter, Margin: 0,20
│   ├── TextBlock (AmmoText) — "30 / 30", FontSize: 20, Color: White
│   └── Image (WeaponIcon) — Size: 48x48
│
├── Image (Crosshair) — Anchored: Center, Size: 16x16
│   └── Texture: crosshair_dot (or crosshair_lines)
│
├── Image (HitMarker) — Anchored: Center, Size: 32x32
│   ├── Texture: hitmarker_x
│   └── Visibility: Hidden
│
├── Image (DamageIndicator) — Anchored: Center, Size: 256x256
│   ├── Texture: damage_indicator_ring
│   └── Visibility: Hidden
│
├── HorizontalBox (TopRight) — Anchored: TopRight, Margin: 20
│   ├── TextBlock (TimeText) — "12:00", FontSize: 18
│   └── Image (DayNightIcon) — Size: 24x24
│
├── VerticalBox (Notifications) — Anchored: TopCenter, Margin: 0,50
│   └── (Dynamic — WBP_NotificationEntry added here)
│
├── HorizontalBox (Compass) — Anchored: TopCenter, Margin: 0,10
│   ├── Image (CompassBg) — Size: 400x30
│   └── Overlay (CompassMarkers)
│
└── TextBlock (DebugInfo) — Anchored: TopLeft, Color: Green
    └── Visibility: Collapsed (F1 toggle)
```

### ProgressBar Settings:
| Bar | Fill Color | Background | Percent |
|-----|-----------|------------|---------|
| Health | #e74c3c (red) | #2c0000 | Bound to SurvivalComp |
| Hunger | #e67e22 (orange) | #2c1500 | Bound to SurvivalComp |
| Thirst | #3498db (blue) | #00152c | Bound to SurvivalComp |
| Stamina | #2ecc71 (green) | #002c0f | Bound to SurvivalComp |

### Event Graph:

**NativeTick** (from C++)
```
→ HealthBar.SetPercent(SurvivalComp.GetHealthPercent())
→ HungerBar.SetPercent(SurvivalComp.GetHungerPercent())
→ ThirstBar.SetPercent(SurvivalComp.GetThirstPercent())
→ StaminaBar.SetPercent(SurvivalComp.GetStaminaPercent())
→ If SurvivalComp.GetHealthPercent() < 0.3:
    → HealthBar.SetFillColorAndOpacity(#ff0000) // pulsing red
    → PlayAnimation("LowHealthPulse")
```

**ShowHitMarker**
```
→ HitMarker.SetVisibility(Visible)
→ PlayAnimation("HitMarkerFade")
→ After 0.3s: HitMarker.SetVisibility(Hidden)
```

**ShowDamageIndicator**
```
→ Calculate angle from player to damage source
→ Rotate DamageIndicator to face source
→ DamageIndicator.SetVisibility(Visible)
→ PlayAnimation("DamageIndicatorFade")
→ After 1.0s: DamageIndicator.SetVisibility(Hidden)
```

**ShowNotification**
```
→ Create WBP_NotificationEntry
→ Set message text
→ Add to Notifications vertical box
→ After Duration seconds: Remove from parent
```

**UpdateCrosshair**
```
→ If bIsAiming:
    → Crosshair.SetVisibility(Visible)
    → Crosshair.SetRenderScale(0.8)
→ Else:
    → Crosshair.SetVisibility(Hidden)
```

---

## 4. WBP_Crafting (Widget Blueprint)

### Parent Class: CraftingWidget (C++)

### Widget Hierarchy:
```
CanvasPanel (Root)
├── Border (Background) — Color: #1a1a1a, Opacity: 0.95
│   └── Overlay
│       ├── HorizontalBox (Header)
│       │   ├── TextBlock — "CRAFTING", FontSize: 24
│       │   ├── Spacer
│       │   ├── EditableTextBox (SearchBox) — Hint: "Search..."
│       │   └── Button (CloseButton) — "X"
│       ├── HorizontalBox (CategoryTabs)
│       │   ├── Button (AllTab) — "ALL"
│       │   ├── Button (WeaponsTab) — "WEAPONS"
│       │   ├── Button (AmmoTab) — "AMMO"
│       │   ├── Button (BuildingTab) — "BUILDING"
│       │   ├── Button (ToolsTab) — "TOOLS"
│       │   ├── Button (MedicalTab) — "MEDICAL"
│       │   └── Button (VehicleTab) — "VEHICLE"
│       ├── HorizontalBox (MainContent)
│       │   ├── ScrollBox (RecipeList) — Width: 300
│       │   │   └── (Dynamic WBP_CraftingEntry)
│       │   └── Border (RecipeDetails) — Width: 400
│       │       └── VerticalBox
│       │           ├── Image (RecipeIcon) — 128x128
│       │           ├── TextBlock (RecipeName) — FontSize: 20, Bold
│       │           ├── TextBlock (RecipeDescription)
│       │           ├── TextBlock (CraftingTime) — "Time: 5.0s"
│       │           ├── TextBlock (RequiredLevel) — "Level: 5"
│       │           ├── TextBlock (IngredientsLabel) — "INGREDIENTS:"
│       │           ├── VerticalBox (IngredientsList)
│       │           │   └── (Dynamic rows: Icon + Name + "Have X / Need Y")
│       │           ├── ProgressBar (CraftingProgress) — Visibility: Hidden
│       │           └── Button (CraftButton) — "CRAFT"
│       └── TextBlock (SelectedStation) — "Station: Workbench"
```

### Event Graph:

**RefreshRecipes**
```
→ Clear RecipeList
→ For each Recipe in AvailableRecipes:
    → If matches SearchBox text AND matches selected category:
        → Create WBP_CraftingEntry
        → Set recipe data
        → Bind OnClicked → SelectRecipe(Recipe)
        → Add to RecipeList
```

**SelectRecipe**
```
→ SelectedRecipe = Recipe
→ RecipeIcon.SetBrushFromTexture(Recipe.Icon)
→ RecipeName.SetText(Recipe.DisplayName)
→ RecipeDescription.SetText(Recipe.Description)
→ CraftingTime.SetText("Time: " + Recipe.CraftingTime + "s")
→ RequiredLevel.SetText("Level: " + Recipe.RequiredLevel)
→ Clear IngredientsList
→ For each Ingredient in Recipe.Ingredients:
    → Create row widget
    → Set icon, name, quantity needed
    → Check inventory: "Have X / Need Y"
    → If X < Y: Set text color red
    → Add to IngredientsList
→ If Recipe.CanCraft(PlayerInventory, PlayerLevel):
    → CraftButton.SetIsEnabled(true)
    → CraftButton.SetBackgroundColor(Green)
→ Else:
    → CraftButton.SetIsEnabled(false)
    → CraftButton.SetBackgroundColor(Red)
```

**CraftButton.OnClicked**
```
→ If SelectedRecipe != null:
    → CraftSelectedRecipe()
    → CraftButton.SetIsEnabled(false)
    → CraftingProgress.SetVisibility(Visible)
    → PlayAnimation("CraftingProgress")
    → After CraftingTime seconds:
        → CraftingProgress.SetVisibility(Hidden)
        → RefreshRecipes()
        → ShowNotification("Crafted: " + Recipe.DisplayName)
```

---

## 5. WBP_DeathScreen (Widget Blueprint)

### Parent Class: DeathScreenWidget (C++)

### Widget Hierarchy:
```
CanvasPanel (Root)
├── Border (Background) — Color: Black, Opacity: 0.9
│   └── Overlay
│       ├── VerticalBox (CenterContent) — Center aligned
│       │   ├── TextBlock (YouDied) — "YOU DIED", FontSize: 72, Color: #e74c3c, Bold
│       │   ├── Spacer (50px)
│       │   ├── TextBlock (KillerText) — "Killed by: Zombie"
│       │   ├── Spacer (20px)
│       │   ├── TextBlock (StatsText) — "Zombies killed: 15\nTime survived: 45 min"
│       │   ├── Spacer (50px)
│       │   ├── TextBlock (RespawnTimer) — "Respawn in: 10"
│       │   ├── Spacer (30px)
│       │   ├── Button (RespawnButton) — "RESPAWN NOW"
│       │   └── Button (MainMenuButton) — "MAIN MENU"
│       └── Image (SkullDecoration) — Anchored: TopCenter, Opacity: 0.1
```

### Animations:
- **FadeIn**: Background opacity 0→0.9 over 1s
- **TextSlide**: YouDied slides from top over 0.5s
- **Pulse**: RespawnTimer pulses every second

---

## 6. WBP_MainMenu (Widget Blueprint)

### Widget Hierarchy:
```
CanvasPanel (Root)
├── Image (Background) — Full screen, game screenshot/video
├── Overlay (Content)
│   ├── VerticalBox (LeftPanel) — Anchored: Left, Margin: 100
│   │   ├── Image (Logo) — Game logo
│   │   ├── Spacer (50px)
│   │   ├── Button (PlayButton) — "PLAY", Size: 300x60
│   │   ├── Button (ServersButton) — "SERVER LIST"
│   │   ├── Button (SettingsButton) — "SETTINGS"
│   │   ├── Button (CreditsButton) — "CREDITS"
│   │   └── Button (QuitButton) — "QUIT"
│   └── VerticalBox (RightPanel) — Anchored: Right, Width: 400
│       ├── TextBlock (PatchNotes) — "PATCH NOTES"
│       └── ScrollBox (PatchNotesContent)
│           └── TextBlock — Latest patch notes text
```

### Event Graph:

**PlayButton.OnClicked**
```
→ Open Level: "Dev_TestMap"
→ Or: Show Server Browser (WBP_ServerBrowser)
```

**ServersButton.OnClicked**
```
→ Create WBP_ServerBrowser
→ Add to viewport
```

**SettingsButton.OnClicked**
```
→ Create WBP_Settings
→ Add to viewport
```

---

## 7. WBP_ServerBrowser (Widget Blueprint)

### Widget Hierarchy:
```
CanvasPanel (Root)
├── Border (Background) — Color: #1a1a1a, Opacity: 0.95
│   └── Overlay
│       ├── VerticalBox (Main)
│       │   ├── HorizontalBox (Header)
│       │   │   ├── TextBlock — "SERVER BROWSER"
│       │   │   ├── Spacer
│       │   │   ├── EditableTextBox (FilterText)
│       │   │   └── Button (RefreshButton) — "🔄"
│       │   ├── ListView (ServerList) — Height: 400
│       │   │   └── (Entry: WBP_ServerEntry)
│       │   └── HorizontalBox (Footer)
│       │       ├── Button (DirectConnect) — "DIRECT CONNECT"
│       │       ├── Button (HostServer) — "HOST SERVER"
│       │       └── Button (BackButton) — "BACK"
│       └── Border (ServerDetails) — Anchored: Right, Width: 300
│           └── VerticalBox
│               ├── TextBlock (ServerName)
│               ├── TextBlock (MapName)
│               ├── TextBlock (Players)
│               ├── TextBlock (Ping)
│               ├── TextBlock (Mode)
│               └── Button (JoinButton) — "JOIN"
```

---

## 8. WBP_Settings (Widget Blueprint)

### Tabs: Video, Audio, Controls, Gameplay

### Video Tab:
- Resolution (ComboBox)
- Fullscreen (CheckBox)
- VSync (CheckBox)
- Graphics Quality (ComboBox: Low/Medium/High/Epic)
- View Distance
- Shadows
- Effects
- Post Processing

### Audio Tab:
- Master Volume (Slider: 0-100)
- SFX Volume (Slider)
- Music Volume (Slider)
- Voice Volume (Slider)

### Controls Tab:
- Mouse Sensitivity (Slider)
- Invert Y (CheckBox)
- Key bindings (rebindable)

### Gameplay Tab:
- Show HUD (CheckBox)
- Show Damage Numbers (CheckBox)
- Field of View (Slider: 70-110)
- Crosshair Style (ComboBox)

---

## 9. WBP_NotificationEntry (Widget Blueprint)

### Widget Hierarchy:
```
Border (Root) — Background: #2c3e50, Padding: 10
└── HorizontalBox
    ├── Image (Icon) — Size: 24x24
    └── TextBlock (Message) — FontSize: 14, Color: White
```

### Animation: "FadeOut"
- Delay: Duration seconds
- Opacity: 1→0 over 0.5s
- Then: Remove from parent

---

## 10. WBP_ItemTooltip (Widget Blueprint)

### Widget Hierarchy:
```
Border (Root) — Background: #1a1a1a, BorderColor: #f39c12, Padding: 10
└── VerticalBox
    ├── TextBlock (ItemName) — FontSize: 16, Bold, Color based on rarity
    ├── TextBlock (ItemType) — FontSize: 10, Gray
    ├── TextBlock (Description) — FontSize: 12, Wrap: true
    └── TextBlock (Stats) — FontSize: 11, Green
```

---

## BINDING WIDGETÓW DO C++

### W SurvivalPlayerController (C++):
```cpp
// In BeginPlay:
if (IsLocalController())
{
    if (HUDWidgetClass)
    {
        CurrentHUD = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        CurrentHUD->AddToViewport();
    }
}
```

### W Project Settings → Maps & Modes:
- **GameMode Default Pawn**: BP_SurvivalCharacter
- **HUD Class**: None (HUD jest w PlayerController)

### Input Actions → UI Toggle:
```
IA_Inventory (Key: Tab)
→ SurvivalPlayerController.ToggleInventory()
→ If InventoryWidget is in viewport: Remove
→ Else: Create WBP_Inventory and AddToViewport
→ Set InputMode: GameAndUI
→ ShowMouseCursor: true
```

```
IA_Crafting (Key: C)
→ SurvivalPlayerController.ToggleCraftingMenu()
```

---

## STYLING (Widget Style)

### Colors:
| Element | Color | Hex |
|---------|-------|-----|
| Background | Dark Gray | #1a1a1a |
| Panel Border | Orange | #e67e22 |
| Button Default | Gray | #34495e |
| Button Hover | Light Gray | #5d6d7e |
| Button Pressed | Dark | #2c3e50 |
| Text Primary | White | #ffffff |
| Text Secondary | Gray | #95a5a6 |
| Health | Red | #e74c3c |
| Success | Green | #2ecc71 |
| Warning | Yellow | #f1c40f |

### Fonts:
- **Headers**: Roboto Bold, Size 24
- **Body**: Roboto Regular, Size 14
- **Small**: Roboto Regular, Size 10

### Materials:
- **UI_Blur**: Post-process blur for backgrounds
- **UI_Gradient**: Subtle gradient overlay
