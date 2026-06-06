#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NOACopilotTypes.generated.h"

UENUM(BlueprintType)
enum class EChapter : uint8
{
    Ch1_CrashSite       UMETA(DisplayName = "Ch1: Crash Site"),
    Ch2_GoingDeeper     UMETA(DisplayName = "Ch2: Going Deeper"),
    Ch3_Abyss           UMETA(DisplayName = "Ch3: The Abyss"),
    Ch4_Unknown         UMETA(DisplayName = "Ch4: Unknown"),
};

UENUM(BlueprintType)
enum class ECategory : uint8
{
    DepthModule     UMETA(DisplayName = "Depth Module"),
    Scannable       UMETA(DisplayName = "Scannable"),
    Blackbox        UMETA(DisplayName = "Blackbox"),
    NOALog          UMETA(DisplayName = "NOA Log"),
    Upgrade         UMETA(DisplayName = "Upgrade"),
    Loot            UMETA(DisplayName = "Loot"),
};

UENUM(BlueprintType)
enum class EEntryStatus : uint8
{
    Hidden      UMETA(DisplayName = "Hidden"),
    Revealed    UMETA(DisplayName = "Revealed"),
    Collected   UMETA(DisplayName = "Collected"),
};

// Row struct for DA_ProgressionDatabase data table.
// Collected state is NOT stored here — it lives in the save component runtime state.
USTRUCT(BlueprintType)
struct NOACOPILOT_API FProgressionEntry : public FTableRowBase
{
    GENERATED_BODY()

    // Unique identifier — must match IDs in act1_items.json and lua hook mappings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    FName ID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    EChapter Chapter = EChapter::Ch1_CrashSite;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    ECategory Category = ECategory::Loot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    FText DisplayName;

    // ID of the NOA terminal dialogue tag or blackbox signal that unlocks this entry
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    FName RevealTrigger;

    // The quote from NOA or the blackbox signal text
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    FText HintText;

    // Human-readable location guidance shown when entry is expanded
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    FText LocationHint;
};
