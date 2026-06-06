#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NOACopilotTypes.h"
#include "NOACopilotManager.generated.h"

class UNOACopilotSaveComponent;
class ANOACopilotMarker;

// Delegate fired when an entry status changes — HUD widget binds to this.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEntryStatusChanged, FName, EntryID, EEntryStatus, NewStatus);

// Persistent actor auto-spawned by UE4SS on session start.
// Owns the runtime state map and is the single source of truth for entry status.
// Lua hooks call RevealEntryFromLua / MarkCollectedFromLua on this actor.
UCLASS(BlueprintType, Blueprintable)
class NOACOPILOT_API ANOACopilotManager : public AActor
{
    GENERATED_BODY()

public:
    ANOACopilotManager();

    // ----------------------------------------------------------------
    // Called by Lua hooks via UE4SS
    // ----------------------------------------------------------------

    UFUNCTION(BlueprintCallable, Category = "NOACopilot")
    void RevealEntryFromLua(const FString& TriggerID);

    UFUNCTION(BlueprintCallable, Category = "NOACopilot")
    void MarkCollectedFromLua(const FString& ItemID);

    // ----------------------------------------------------------------
    // Blueprint-callable query API (used by HUD widget)
    // ----------------------------------------------------------------

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NOACopilot")
    EEntryStatus GetEntryStatus(FName EntryID) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NOACopilot")
    TArray<FProgressionEntry> GetEntriesForChapter(EChapter Chapter) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NOACopilot")
    TArray<FProgressionEntry> GetRevealedEntries() const;

    // ----------------------------------------------------------------
    // Map marker API — called by WBP_ProgressionEntry pin button
    // ----------------------------------------------------------------

    // Spawn a world marker at the entry's WorldLocation. Replaces any existing marker for this entry.
    // No-op if entry has no WorldLocation set (zero vector).
    UFUNCTION(BlueprintCallable, Category = "NOACopilot")
    void PlaceMarker(FName EntryID);

    // Remove the marker for this entry if one exists.
    UFUNCTION(BlueprintCallable, Category = "NOACopilot")
    void ClearMarker(FName EntryID);

    // Returns true if a live marker exists for this entry — used to toggle pin button state.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NOACopilot")
    bool HasMarker(FName EntryID) const;

    // ----------------------------------------------------------------
    // Events — HUD widget and save component bind to these
    // ----------------------------------------------------------------

    // Fired when a marker is placed or cleared — HUD pin button binds to this to update its state.
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMarkerChanged, FName, EntryID, bool, bPlaced);
    UPROPERTY(BlueprintAssignable, Category = "NOACopilot")
    FOnMarkerChanged OnMarkerChanged;

    UPROPERTY(BlueprintAssignable, Category = "NOACopilot")
    FOnEntryStatusChanged OnEntryStatusChanged;

    // ----------------------------------------------------------------
    // Data
    // ----------------------------------------------------------------

    // Set this in Blueprint defaults to DA_ProgressionDatabase
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NOACopilot")
    UDataTable* ProgressionDatabase;

    // Set this in Blueprint defaults to BP_NOACopilotMarker
    UPROPERTY(EditDefaultsOnly, Category = "NOACopilot")
    TSubclassOf<ANOACopilotMarker> MarkerClass;

protected:
    virtual void BeginPlay() override;

private:
    // Runtime status map — populated from save on BeginPlay, updated by hooks
    UPROPERTY()
    TMap<FName, EEntryStatus> EntryStatusMap;

    // Maps RevealTrigger IDs → list of entry IDs they unlock
    TMultiMap<FName, FName> TriggerToEntryMap;

    UPROPERTY()
    UNOACopilotSaveComponent* SaveComponent;

    void BuildTriggerMap();
    void SetEntryStatus(FName EntryID, EEntryStatus NewStatus);

    // Live marker actors keyed by entry ID
    UPROPERTY()
    TMap<FName, ANOACopilotMarker*> ActiveMarkers;
};
