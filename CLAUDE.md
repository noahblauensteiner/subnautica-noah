# CLAUDE.md — NOA Copilot Mod Handover

This file is for the next Claude Code agent picking up this project. Read it before touching anything.

---

## What this project is

A **Subnautica 2 mod** (Unreal Engine 5.6, not Unity) that adds a persistent HUD panel tracking story-critical missables. Items only surface in the tracker after the game reveals them via NOA terminal dialogue or blackbox signal — no spoilers, no full checklists upfront.

## Where we left off

**Phases 1 and 2 are complete** (documented, not yet built in UE — they represent planning + data deliverables):
- `plan.spec.md` — full spec with status checkboxes, update it as you go
- `act1_items.json` — 7 Act 1 entries across all categories, wiki-sourced, needs in-game verification
- `lua/noa_copilot_hooks.lua` — UE4SS Lua scaffold, class names are placeholder TODOs

**Phase 3 is next:** Manager & Reveal System inside UE 5.6.1.

## The single most important first step

Run the **UE4SS C++ dumper** in-game before writing any Blueprint logic:
- Launch Subnautica 2 with UE4SS injected (`Subnautica2/Binaries/Win64/`)
- Open UE4SS console with `Ctrl+H`
- Dump SDK to get real class names for: NOA terminal component, blackbox signal component, pickup component, scanner tool, and the root HUD widget class
- Replace all TODOs in `lua/noa_copilot_hooks.lua` with the real names

Everything in Phase 3 onward depends on these class names.

## Key design decisions already made

| Decision | Choice | Reason |
|---|---|---|
| Reveal gating | Items hidden until NOA/blackbox event | No spoilers; respects game pacing |
| Data source | Hardcoded JSON + Data Table, community-editable | Resilient to patches without code deploys |
| HUD position | Left navbar, above health/oxygen | Matches game's existing UI cluster |
| Panel behaviour | Expandable, collapsed by default | Minimal screen clutter |
| Organization | By story chapter/act | Matches narrative flow |
| Multiplayer | Solo only for v1 | Keeps save architecture simple |
| Collection detection | Both real-time hooks + save sync | Real-time UX + session continuity |

## Architecture summary

```
BP_NOACopilotManager          — Persistent actor, auto-spawned by UE4SS
  └── BP_NOACopilotSaveComponent  — Save/load of completion state

BP_NOACopilotHUD              — UMG widget anchored left edge
  ├── WBP_ChapterSection      — Collapsible chapter group
  └── WBP_ProgressionEntry    — Item row: icon, status, expand → hint + location

DA_ProgressionDatabase        — Data Table (FProgressionEntry rows)
lua/noa_copilot_hooks.lua     — UE4SS Lua: intercepts pickup, scan, NOA, blackbox events
```

Runtime state lives in the Manager as `Map<FName, EEntryStatus>` (Hidden → Revealed → Collected).
Collected state is NOT in the Data Table — it lives in the save component.

## Lua → Blueprint bridge

The Lua hooks call two Blueprint functions exposed on `BP_NOACopilotManager`:
- `RevealEntryFromLua(FName id)` — triggered by NOA terminal / blackbox events
- `MarkCollectedFromLua(FName id)` — triggered by pickup / scan events

## Data files

- `act1_items.json` — source of truth for Act 1 entries, edit this when adding/fixing items
- `DA_ProgressionDatabase` (UE asset) — must be kept in sync with the JSON manually or via an import script

## Risks to watch

1. **HUD injection** — don't replace the root HUD widget, add as a child. Find the root class via dumper first.
2. **Patch fragility** — Lua hooks break when UE4SS updates after game patches. Blueprint logic is more stable.
3. **Reveal trigger IDs** — `RevealTrigger` values in `act1_items.json` are guesses. Verify against real in-game event tags via dumper.

## Tools needed

- Unreal Engine 5.6.1 + Visual Studio 2022 (MSVC v14.38)
- UE4SS — `Subnautica2Modding/Subnautica2-UE4SS`
- Mariana framework — `GOD-GAMER/mariana-sn2`
- SN2ModSettings
- FMOD for Unreal (required by the template project)

## Repo / session context

- Original planning session: Claude Code on the web, session `019pPCvsoftPPuXYfe2tJ344`
- Working branch convention: `claude/<name>` 
- Always update `plan.spec.md` checkboxes as phases complete
