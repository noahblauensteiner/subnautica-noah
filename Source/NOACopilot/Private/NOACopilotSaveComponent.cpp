#include "NOACopilotSaveComponent.h"
#include "Kismet/GameplayStatics.h"

// TODO Phase 5: replace USaveGame stub with a real save object once we know
// how Subnautica 2 exposes its save system (check dumped classes for SaveManager or similar)

UNOACopilotSaveComponent::UNOACopilotSaveComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UNOACopilotSaveComponent::BeginPlay()
{
    Super::BeginPlay();
    // TODO Phase 5: bind to game save delegate so FlushSave fires at the right time
}

void UNOACopilotSaveComponent::LoadAndApply(TMap<FName, EEntryStatus>& OutStatusMap)
{
    // TODO Phase 5: load from save slot and overlay onto OutStatusMap
    // For now this is a no-op — state resets each session
    UE_LOG(LogTemp, Warning, TEXT("[NOACopilot] Save load not yet implemented (Phase 5)"));
}

void UNOACopilotSaveComponent::MarkDirty()
{
    bDirty = true;
    // TODO Phase 5: trigger autosave or queue a flush
}

void UNOACopilotSaveComponent::FlushSave(const TMap<FName, EEntryStatus>& StatusMap)
{
    // TODO Phase 5: serialise StatusMap and write to SaveSlotName
}
