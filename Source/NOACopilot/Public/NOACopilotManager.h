#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NOACopilotTypes.h"
#include "NOACopilotManager.generated.h"

class UNOACopilotSaveComponent;

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
    // Events — HUD widget and save component bind to these
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "NOACopilot")
    FOnEntryStatusChanged OnEntryStatusChanged;

    // ----------------------------------------------------------------
    // Data
    // ----------------------------------------------------------------

    // Set this in Blueprint defaults to DA_ProgressionDatabase
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NOACopilot")
    UDataTable* ProgressionDatabase;

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
};
