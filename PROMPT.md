# Subnautica 2 Modding — Session Handover Prompt

Use this file to brief a new Claude Code agent or continue work in a new session. Paste the contents into the chat to restore full context.

---

## Project overview

We are building **one or two mods for Subnautica 2** (Unreal Engine 5.6, Early Access as of June 2026). The game uses UE4SS as its modding runtime — no BepInEx, no Nautilus, no Unity. Mods are built in UE 5.6.1 using the `Subnautica2Modding/Subnautica2-Project` template and packaged as pak chunks.

**Repo:** `github.com/noahblauensteiner/subnautica-noah`
**Branch:** `main`
**Planning doc:** `plan.spec.md` (has status checkboxes — update as you go)

---

## Tech stack

| Layer | Tool |
|---|---|
| Engine | Unreal Engine 5.6.1 |
| IDE | Visual Studio 2022, MSVC v14.38 |
| Mod loader | UE4SS + Blueprint Mod Loader |
| Item/recipe registration | Mariana framework (`GOD-GAMER/mariana-sn2`) |
| UI config | SN2ModSettings |
| Audio | FMOD for Unreal (required by template) |
| Data | Community-editable JSON → UE Data Table |

---

## Mod Idea 1 — NOA Copilot (active, in progress)

### What it is
A persistent HUD panel on the **left navbar above the health/oxygen bar** that tracks story-critical missables: blackboxes, depth modules, scannables, NOA logs, upgrades, and loot.

**Core rule:** Items only appear in the tracker **after the game has told the player about them** via a NOA terminal dialogue or blackbox signal. No spoilers, no full upfront checklist.

### UX behaviour
- **Collapsed:** small tab showing `Chapter X/Y` count
- **Expanded:** chapter-grouped list, each chapter collapsible
- **Entry click:** expands inline to show the original NOA/blackbox hint text + a location guide
- **Unrevealed entries:** hidden entirely
- **Revealed but uncollected:** pulse gently
- **Collected:** checkmark / strikethrough
- **Pin button:** click to drop a **world-space map marker** at the item's coordinates, scanner-room style. Click again to remove. Auto-clears on collection.

### Data
- `act1_items.json` — source of truth for Act 1 entries (blackboxes, depth modules, blueprints, POIs) with real coordinates from verified guides (patch v0.1.4)
- `blueprints.json` — all 13 confirmed blueprint locations with exact X,Y,Z coords and recipes
- `DA_ProgressionDatabase` UE Data Table — row struct `FProgressionEntry`

### Architecture

```
ANOACopilotManager (persistent actor, auto-spawned by UE4SS)
  ├── TMap<FName, EEntryStatus>     — runtime state (Hidden → Revealed → Collected)
  ├── TMultiMap<FName, FName>       — RevealTrigger → EntryIDs
  ├── TMap<FName, ANOACopilotMarker*> — active map markers
  ├── RevealEntryFromLua(TriggerID) — called by Lua hooks
  ├── MarkCollectedFromLua(ItemID)  — called by Lua hooks
  ├── PlaceMarker(EntryID)          — spawns world beacon at WorldLocation
  ├── ClearMarker(EntryID)          — destroys beacon
  ├── OnEntryStatusChanged          — delegate, HUD widget binds to this
  └── OnMarkerChanged               — delegate, pin button binds to this

UNOACopilotSaveComponent (attached to Manager)
  └── Load/save completion state across sessions (stub — Phase 5)

ANOACopilotMarker (world actor)
  ├── BeaconMesh + PointLight
  ├── RegisterWithMap()             — BlueprintImplementableEvent (TODO: wire to game's beacon system)
  └── UnregisterFromMap()

BP_NOACopilotHUD (UMG widget, left edge anchor)
  ├── WBP_ChapterSection            — collapsible chapter group
  └── WBP_ProgressionEntry          — row: icon, status, expand button, pin button

lua/noa_copilot_hooks.lua (UE4SS Lua)
  — hooks into pickup, scan, NOA terminal, blackbox signal events
  — calls RevealEntryFromLua / MarkCollectedFromLua on the manager
  — ALL class names are TODO placeholders until UE4SS dump is done
```

### C++ files written (drop into UE template Source/)
- `Source/NOACopilot/Public/NOACopilotTypes.h` — enums + FProgressionEntry struct
- `Source/NOACopilot/Public/NOACopilotManager.h` + `Private/NOACopilotManager.cpp`
- `Source/NOACopilot/Public/NOACopilotSaveComponent.h` + `Private/NOACopilotSaveComponent.cpp`
- `Source/NOACopilot/Public/NOACopilotMarker.h` + `Private/NOACopilotMarker.cpp`

### Current phase status
See `plan.spec.md` for full checklist. Summary:

| Phase | Status | Notes |
|---|---|---|
| 1 — Scaffold & widget anchor | `[x]` | Documented |
| 2 — Progression database | `[x]` | Data in act1_items.json |
| 3 — Manager & reveal system | `[~]` | C++ done; needs in-game dumper run |
| 4 — Real-time collection hook | `[ ]` | |
| 5 — Save/load sync | `[ ]` | |
| 6 — Full HUD widget | `[ ]` | |
| 6a — Map marker system | `[ ]` | C++ done; needs scanner class name from dump |
| 7 — NOA terminal event wiring | `[ ]` | |
| 8 — Package & polish | `[ ]` | |

### Biggest blocker right now
**UE4SS CXX Header Dump** — the player has run `Dump CXX Headers` from the UE4SS Debugging Tools panel. Output is at:
```
C:\Program Files (x86)\Steam\steamapps\common\Subnautica2\Subnautica2\Binaries\Win64\ue4ss\CXXHeaderDump\
```
We need to search the dump for the real class names of:
- NOA terminal component (search: `NOA`)
- Blackbox signal component (search: `BlackBox`)
- Pickup component (search: `PickUp`, `Collect`)
- Scanner tool (search: `Scanner`)
- Map marker / beacon (search: `Beacon`, `MapMarker`, `MapIcon`)
- Root HUD widget (search: `PlayerHUD`, `GameHUD`, `HUDWidget`)

Once those class names are found, replace the TODOs in `lua/noa_copilot_hooks.lua` and wire `ANOACopilotMarker.RegisterWithMap()` to the real beacon system.

PowerShell search command (run in player's terminal):
```powershell
$dump = "C:\Program Files (x86)\Steam\steamapps\common\Subnautica2\Subnautica2\Binaries\Win64\ue4ss\CXXHeaderDump"
Select-String -Path "$dump\*.hpp" -Pattern "NOA" -List | Select-Object Filename
```
(Repeat for each search term above.)

---

## Mod Idea 2 — Storage Terminal (explored, not started)

### What it is
A new **interior facility buildable** (like the fabricator) called the **Storage Terminal**. It aggregates the inventories of all lockers connected to the same base and lets you browse, search, and retrieve items from one place.

### Why it was deprioritised
Both mods were scoped, but NOA Copilot was chosen to build first because it has clearer progression and less dependency on unknown inventory class names.

### Architecture (designed, no code written)
```
BP_StorageNetworkComponent (Actor Component on the terminal)
  — scans for StorageContainer actors within base radius on timer
  — aggregates all found inventories into a unified virtual view

BP_StorageTerminal (Blueprint Actor — the buildable)
  — static mesh, interaction opens BP_StorageTerminalWidget
  — registered with Mariana framework so habitat builder discovers it

BP_StorageTerminalWidget (UMG)
  — grid/list of all networked items: icon, name, quantity
  — search bar filter
  — click item → TransferItemToPlayer from source container
```

Mariana registration (Lua):
```lua
Items.Register({ id = "StorageTerminal", blueprint = "/Game/Mods/StorageTerminal/BP_StorageTerminal", category = "InteriorFacilities" })
Crafting.AddRecipe({ item = "StorageTerminal", ingredients = { { id = "Titanium", count = 3 }, { id = "WiringKit", count = 1 } } })
```

### Key unknowns before starting
- Real storage container class name (needs UE4SS dump — same dump session, search `StorageContainer`, `ItemsContainer`, `Inventory`)
- Whether Mariana exposes a "same base segment" graph query or if radius scanning is the only option
- Multiplayer authority for the merged inventory view

---

## Data files in repo

| File | Contents |
|---|---|
| `plan.spec.md` | Full phased plan with checkboxes |
| `act1_items.json` | 6 blackboxes, 2 depth modules, 7 key blueprints, 9 POIs with real coords |
| `blueprints.json` | All 13 blueprint locations, full coords, recipes, alt scan spots |
| `CLAUDE.md` | Condensed agent handover (architecture, decisions, first steps) |
| `lua/noa_copilot_hooks.lua` | UE4SS Lua hook scaffold (class names are TODOs) |
| `Source/NOACopilot/` | C++ source for Manager, Marker, SaveComponent, Types |

---

## Key design decisions (don't re-litigate these)

| Decision | Choice |
|---|---|
| Reveal gating | Hidden until NOA/blackbox event — no spoilers |
| Data source | Hardcoded JSON + UE Data Table, community-editable |
| HUD position | Left navbar, above health/oxygen |
| Panel UX | Expandable, collapsed by default |
| Organisation | By story chapter/act |
| Multiplayer | Solo only for v1 |
| Collection detection | Real-time hooks + save sync both |
| Marker style | Scanner room beacon style, auto-clear on collect |
