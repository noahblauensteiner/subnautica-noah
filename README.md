# NOA Copilot

A Subnautica 2 mod that tracks story-critical missables — blackbox fragments, depth modules, scannables, NOA logs, and more — in a persistent HUD panel on the left side of the screen.

Items only appear in the tracker **after the game has told you about them** via a NOA terminal or blackbox signal. Each entry shows its completion status and expands on click to show the original hint and a location guide.

---

## What it does

- Persistent collapsible panel anchored above the health/oxygen bar
- Items grouped by story chapter/act
- Entries are hidden until revealed by in-game events (NOA terminals, blackbox signals)
- Click any entry to expand hint text and location guidance
- Tracks collection in real-time via pickup/scan hooks
- State persists across sessions via save sync

## Status

Early development — see `plan.spec.md` for the full phased plan and current progress.

**Phases 1–2 complete** (scaffold + data model)
**Phase 3 next** (Manager & Reveal System — requires UE4SS dumper run in-game)

## Tech stack

- Unreal Engine 5.6.1
- UE4SS (injectable Lua runtime)
- Mariana framework (item/event registration)
- SN2ModSettings (keybind UI)
- FMOD for Unreal

## Repo structure

```
plan.spec.md              — Phased plan with status checkboxes
act1_items.json           — Act 1 progression entries (wiki-sourced, needs in-game verification)
lua/
  noa_copilot_hooks.lua   — UE4SS Lua hook scaffold (class names are TODOs until dumper run)
```

The UE5 Blueprint assets live in `Content/Mods/NOACopilot/` inside the `Subnautica2-Project` template — see `plan.spec.md` for the full file map.

## Getting started

1. Clone `Subnautica2Modding/Subnautica2-Project` and open in UE 5.6.1
2. Create `Content/Mods/NOACopilot/` and follow the file map in `plan.spec.md`
3. Run UE4SS in-game and use the C++ dumper (Ctrl+H) to find real class names
4. Replace TODOs in `lua/noa_copilot_hooks.lua` with dumped names
5. Pick up from Phase 3 in `plan.spec.md`
