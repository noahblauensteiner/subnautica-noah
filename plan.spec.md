# NOA Copilot — Mod Plan

> Subnautica 2 story progression tracker HUD. Items appear only after the game reveals them via NOA terminal dialogue or blackbox signal. Persistent left-navbar panel above health/oxygen bar.

---

## Status Legend
- `[ ]` Not started
- `[~]` In progress
- `[x]` Done

---

## Phases

### Phase 1 — Scaffold & Widget Anchor `[x]`
- [x] Clone `Subnautica2Modding/Subnautica2-Project` (UE 5.6.1)
- [x] Create `Content/Mods/NOACopilot/` folder
- [x] Create `PAL_NOACopilot` Primary Asset Label (Chunk ID: pick unused 1–300, Cook Rule: Always Cook)
- [x] Stub `BP_NOACopilotManager` actor (empty, auto-spawned by UE4SS on session start)
- [x] Stub `BP_NOACopilotHUD` UMG widget anchored to left edge, above health/oxygen cluster
- [x] Verify widget appears in-game (hardcoded "NOA Copilot" label as smoke test)
- [x] Identify root HUD widget class via UE4SS C++ dumper (Ctrl+H in console) — needed for injection point

### Phase 2 — Progression Database (Act 1) `[x]`
- [x] Create `DA_ProgressionDatabase` as a Data Table using `FProgressionEntry` row struct
- [x] Define `FProgressionEntry` struct (see Data Model section below)
- [x] Define `EChapter` enum (Ch1_CrashSite, Ch2_GoingDeeper, Ch3_Abyss …)
- [x] Define `ECategory` enum (DepthModule, Scannable, Blackbox, NOALog, Upgrade, Loot)
- [x] Populate Act 1 rows from wiki research (see `act1_items.json` for raw data)
- [x] Validate table loads without errors in UE editor

### Phase 3 — Manager & Reveal System `[~]`
- [x] Define `FProgressionEntry`, `EChapter`, `ECategory`, `EEntryStatus` — `Source/NOACopilot/Public/NOACopilotTypes.h`
- [x] Implement `ANOACopilotManager` C++ skeleton — reveal/collect logic, trigger map, query API, status change delegate
- [x] Stub `UNOACopilotSaveComponent` — load/flush placeholders (real impl in Phase 5)
- [ ] **REQUIRES IN-GAME:** Run UE4SS C++ dumper (Ctrl+H) to find real class names for NOA terminal, blackbox signal, pickup, scanner, root HUD widget
- [ ] Replace TODOs in `lua/noa_copilot_hooks.lua` with dumped class names
- [ ] Create `BP_NOACopilotManager` Blueprint subclass in UE editor, assign `DA_ProgressionDatabase`
- [ ] Test: call `RevealEntryFromLua` / `MarkCollectedFromLua` from UE4SS console, verify `OnEntryStatusChanged` fires
- [ ] Newly revealed entries play a subtle slide-in animation (wire in Phase 6 HUD work)

### Phase 4 — Real-time Collection Hook `[ ]`
- [ ] Write UE4SS Lua script to intercept `OnItemPickedUp(itemID)` and `OnScanCompleted(entityID)`
- [ ] Lua calls Blueprint dispatcher → `BP_NOACopilotManager.MarkCollected(ID: FName)`
- [ ] Collected entries update status (checkmark/strikethrough) immediately
- [ ] Test with a known Act 1 scannable

### Phase 5 — Save/Load Sync `[ ]`
- [ ] Implement `BP_NOACopilotSaveComponent` attached to Manager
- [ ] On session start: read game save data, diff against runtime state, patch any gaps
- [ ] On game save event: write NOACopilot completion state into save slot (custom save object or appended data)
- [ ] Test: collect item, quit, reload — verify entry still marked collected

### Phase 6 — Full HUD Widget `[ ]`
- [ ] `BP_NOACopilotHUD`: collapsed state = small tab showing `ChapterName X/Y`
- [ ] Expand/collapse on click or configurable keybind (via SN2ModSettings)
- [ ] `WBP_ChapterSection`: collapsible chapter group with header + item count
- [ ] `WBP_ProgressionEntry`: icon, name, status indicator, expand button
- [ ] Expand entry → inline detail box shows `HintText` + `LocationHint`
- [ ] Uncollected revealed entries pulse gently
- [ ] Scroll if panel overflows screen height

### Phase 7 — NOA Terminal & Blackbox Event Wiring `[ ]`
- [ ] Map every `RevealTrigger` ID in the database to a real in-game event/tag
- [ ] Verify all Act 1 reveals fire correctly during a full playthrough
- [ ] Handle edge case: player somehow collects item before reveal (silently mark collected, reveal when triggered)

### Phase 8 — Package & Polish `[ ]`
- [ ] Package as pak chunk (.pak + .ucas + .utoc)
- [ ] Install to `Subnautica2/Content/Paks/LogicMods/NOACopilot/`
- [ ] Full playthrough smoke test (Act 1 complete)
- [ ] Performance check: HUD widget cost at 60fps
- [ ] README for mod page (CurseForge / Nexus)
- [ ] Publish v0.1.0

---

## Data Model

### FProgressionEntry (Data Table Row Struct)

```cpp
USTRUCT(BlueprintType)
struct FProgressionEntry : public FTableRowBase
{
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName      ID;             // e.g. "depth_module_mk2"
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EChapter   Chapter;        // Ch1_CrashSite ...
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ECategory  Category;       // DepthModule, Scannable ...
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText      DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName      RevealTrigger;  // NOA terminal or blackbox ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText      HintText;       // Quote from NOA / signal
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText      LocationHint;   // "Swim south ~200m depth"
    // Collected state is NOT stored here — lives in save component runtime state
};
```

### Runtime State (in BP_NOACopilotManager)

```
Map<FName, EEntryStatus>  where EEntryStatus = Hidden | Revealed | Collected
```

---

## File Map

```
Content/Mods/NOACopilot/
├── BP_NOACopilotManager.uasset          ← Persistent actor, auto-spawned by UE4SS
├── BP_NOACopilotSaveComponent.uasset    ← Save/load component attached to Manager
├── BP_NOACopilotHUD.uasset             ← Root UMG widget, left-panel anchor
│   ├── WBP_ChapterSection.uasset       ← Collapsible chapter group
│   └── WBP_ProgressionEntry.uasset     ← Single item row with expand/collapse
├── DA_ProgressionDatabase.uasset       ← Data Table (FProgressionEntry rows)
├── ST_ProgressionEntry.uasset          ← Struct definition for table rows
├── E_Chapter.uasset                    ← Chapter enum
├── E_Category.uasset                   ← Category enum
└── PAL_NOACopilot.uasset               ← Primary Asset Label (chunk cook config)

noa-copilot/                            ← This repo folder (data + planning)
├── plan.spec.md                        ← This file
├── act1_items.json                     ← Raw Act 1 progression data (wiki-sourced)
└── lua/
    └── noa_copilot_hooks.lua           ← UE4SS Lua hook script
```

---

## Known Risks

| Risk | Mitigation |
|---|---|
| Reveal trigger hookpoint unknown | Use UE4SS dumper early in Phase 3 to find NOA terminal delegate |
| HUD injection breaks native UI | Find root HUD widget class first (Phase 1 last step); add widget as child, don't replace |
| Game patches break hooks | Lua hooks go through UE4SS which updates per patch; Blueprint logic is more stable |
| Story data shifts in Early Access | All story data lives in `act1_items.json` + Data Table — update without code changes |
| Multiplayer (future) | v1 is solo-only; save component architecture is already player-scoped for easy future extension |

---

## Tools & Dependencies

- Unreal Engine 5.6.1
- Visual Studio 2022 (MSVC v14.38)
- UE4SS (injectable Lua runtime) — `Subnautica2Modding/Subnautica2-UE4SS`
- Mariana framework (item/event registration) — `GOD-GAMER/mariana-sn2`
- SN2ModSettings (keybind config UI)
- FMOD for Unreal (audio, required by template)
