#include "NOACopilotManager.h"
#include "NOACopilotSaveComponent.h"

ANOACopilotManager::ANOACopilotManager()
{
    PrimaryActorTick.bCanEverTick = false;

    SaveComponent = CreateDefaultSubobject<UNOACopilotSaveComponent>(TEXT("SaveComponent"));
}

void ANOACopilotManager::BeginPlay()
{
    Super::BeginPlay();

    if (!ProgressionDatabase)
    {
        UE_LOG(LogTemp, Error, TEXT("[NOACopilot] ProgressionDatabase is not set on ANOACopilotManager!"));
        return;
    }

    // Initialise all entries as Hidden
    ProgressionDatabase->ForeachRow<FProgressionEntry>(TEXT("NOACopilotManager::BeginPlay"),
        [this](const FName& Key, const FProgressionEntry& Row)
        {
            EntryStatusMap.Add(Row.ID, EEntryStatus::Hidden);
        });

    BuildTriggerMap();

    // Overlay save state — marks previously collected/revealed entries
    SaveComponent->LoadAndApply(EntryStatusMap);
}

void ANOACopilotManager::BuildTriggerMap()
{
    TriggerToEntryMap.Empty();

    ProgressionDatabase->ForeachRow<FProgressionEntry>(TEXT("NOACopilotManager::BuildTriggerMap"),
        [this](const FName& Key, const FProgressionEntry& Row)
        {
            if (!Row.RevealTrigger.IsNone())
            {
                TriggerToEntryMap.Add(Row.RevealTrigger, Row.ID);
            }
        });
}

void ANOACopilotManager::RevealEntryFromLua(const FString& TriggerID)
{
    const FName TriggerName(*TriggerID);
    TArray<FName> EntryIDs;
    TriggerToEntryMap.MultiFind(TriggerName, EntryIDs);

    for (const FName& EntryID : EntryIDs)
    {
        // Only promote Hidden → Revealed, never demote Collected
        if (GetEntryStatus(EntryID) == EEntryStatus::Hidden)
        {
            SetEntryStatus(EntryID, EEntryStatus::Revealed);
        }
    }
}

void ANOACopilotManager::MarkCollectedFromLua(const FString& ItemID)
{
    const FName EntryID(*ItemID);

    // Accept collection even if not yet Revealed (edge case: player finds it before NOA tells them)
    if (EntryStatusMap.Contains(EntryID))
    {
        SetEntryStatus(EntryID, EEntryStatus::Collected);
    }
}

EEntryStatus ANOACopilotManager::GetEntryStatus(FName EntryID) const
{
    if (const EEntryStatus* Status = EntryStatusMap.Find(EntryID))
    {
        return *Status;
    }
    return EEntryStatus::Hidden;
}

TArray<FProgressionEntry> ANOACopilotManager::GetEntriesForChapter(EChapter Chapter) const
{
    TArray<FProgressionEntry> Result;
    if (!ProgressionDatabase) return Result;

    ProgressionDatabase->ForeachRow<FProgressionEntry>(TEXT("GetEntriesForChapter"),
        [&](const FName& Key, const FProgressionEntry& Row)
        {
            if (Row.Chapter == Chapter)
            {
                Result.Add(Row);
            }
        });

    return Result;
}

TArray<FProgressionEntry> ANOACopilotManager::GetRevealedEntries() const
{
    TArray<FProgressionEntry> Result;
    if (!ProgressionDatabase) return Result;

    ProgressionDatabase->ForeachRow<FProgressionEntry>(TEXT("GetRevealedEntries"),
        [&](const FName& Key, const FProgressionEntry& Row)
        {
            EEntryStatus Status = GetEntryStatus(Row.ID);
            if (Status == EEntryStatus::Revealed || Status == EEntryStatus::Collected)
            {
                Result.Add(Row);
            }
        });

    return Result;
}

void ANOACopilotManager::SetEntryStatus(FName EntryID, EEntryStatus NewStatus)
{
    EntryStatusMap.Add(EntryID, NewStatus);
    OnEntryStatusChanged.Broadcast(EntryID, NewStatus);
    SaveComponent->MarkDirty();
}
