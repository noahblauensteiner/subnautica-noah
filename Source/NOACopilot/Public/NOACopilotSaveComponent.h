#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NOACopilotTypes.h"
#include "NOACopilotSaveComponent.generated.h"

// Handles persistence of entry completion state across sessions.
// Attached to ANOACopilotManager.
UCLASS(ClassGroup=(NOACopilot), meta=(BlueprintSpawnableComponent))
class NOACOPILOT_API UNOACopilotSaveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNOACopilotSaveComponent();

    // Called by Manager on BeginPlay — overlays saved state onto the runtime map
    void LoadAndApply(TMap<FName, EEntryStatus>& OutStatusMap);

    // Called by Manager whenever an entry status changes
    void MarkDirty();

    // Save slot name — change this if you want per-profile saves
    UPROPERTY(EditDefaultsOnly, Category = "NOACopilot")
    FString SaveSlotName = TEXT("NOACopilotSave");

protected:
    virtual void BeginPlay() override;

private:
    bool bDirty = false;

    // Flush to disk on game save event
    // TODO Phase 5: hook into the game's save delegate here
    void FlushSave(const TMap<FName, EEntryStatus>& StatusMap);
};
