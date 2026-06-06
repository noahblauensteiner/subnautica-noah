-- NOA Copilot — UE4SS Lua hook script
-- Intercepts pickup and scan events, notifies BP_NOACopilotManager via dispatcher.
--
-- SETUP: Drop this file into ue4ss/Mods/NOACopilot/Scripts/main.lua
-- REQUIRES: UE4SS injected into Subnautica2/Binaries/Win64/
--
-- TODO Phase 3: Replace placeholder class/function names below with real ones
-- found via the UE4SS C++ dumper (Ctrl+H in UE4SS console in-game).

local manager = nil  -- cached reference to BP_NOACopilotManager instance

-- Resolve the persistent manager actor once the world is ready.
local function FindManager()
    if manager ~= nil then return manager end
    -- TODO: replace "BP_NOACopilotManager_C" with exact class name from dumper
    local actors = FindAllOf("BP_NOACopilotManager_C")
    if actors and #actors > 0 then
        manager = actors[1]
    end
    return manager
end

-- Called when the player picks up an item.
-- itemID should map to a row ID in DA_ProgressionDatabase.
-- TODO: find the real pickup delegate name via dumper and replace hook target below.
NotifyOnNewObject("/Script/Subnautica2.PickupComponent", function(pickup_component)
    pickup_component:RegisterHook("OnItemPickedUp", function(self, item_id)
        local mgr = FindManager()
        if mgr == nil then return end
        -- Dispatch to Blueprint — BP_NOACopilotManager exposes MarkCollectedFromLua(FName)
        -- TODO: verify function name matches Blueprint event dispatcher name
        mgr:MarkCollectedFromLua(tostring(item_id))
    end)
end)

-- Called when the player completes a scan.
-- TODO: find the real scan completion delegate name via dumper.
NotifyOnNewObject("/Script/Subnautica2.ScannerTool", function(scanner)
    scanner:RegisterHook("OnScanCompleted", function(self, entity_id)
        local mgr = FindManager()
        if mgr == nil then return end
        mgr:MarkCollectedFromLua(tostring(entity_id))
    end)
end)

-- Called when a NOA terminal plays a dialogue line.
-- RevealTrigger IDs in the database must match the tag strings used here.
-- TODO: find how dialogue tags are broadcast (likely a GameplayTag or named event).
NotifyOnNewObject("/Script/Subnautica2.NOATerminalComponent", function(terminal)
    terminal:RegisterHook("OnDialogueTagPlayed", function(self, tag)
        local mgr = FindManager()
        if mgr == nil then return end
        mgr:RevealEntryFromLua(tostring(tag))
    end)
end)

-- Called when a blackbox signal is received/logged.
-- TODO: find the signal event class/delegate name via dumper.
NotifyOnNewObject("/Script/Subnautica2.BlackboxSignalComponent", function(signal)
    signal:RegisterHook("OnSignalReceived", function(self, signal_id)
        local mgr = FindManager()
        if mgr == nil then return end
        mgr:RevealEntryFromLua(tostring(signal_id))
    end)
end)

print("[NOACopilot] Hook script loaded. Waiting for manager actor...")
