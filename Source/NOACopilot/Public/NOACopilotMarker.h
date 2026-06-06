#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NOACopilotMarker.generated.h"

// World-space marker placed when the player pins a todo entry from the HUD panel.
// Mimics the scanner room marker: visible beacon in the world + icon on the PDA map.
//
// TODO Phase 6a: After running the UE4SS dumper, check if the game has a base beacon/marker
// class (e.g. ABeaconMarker, AScannerRoomTarget) and reparent this Blueprint subclass to it
// instead of AActor — that gets map registration for free.
UCLASS(BlueprintType, Blueprintable)
class NOACOPILOT_API ANOACopilotMarker : public AActor
{
    GENERATED_BODY()

public:
    ANOACopilotMarker();

    // Set by Manager immediately after spawning
    UPROPERTY(BlueprintReadOnly, Category = "NOACopilot")
    FName EntryID;

    UPROPERTY(BlueprintReadOnly, Category = "NOACopilot")
    FText Label;

    // The beacon mesh component — swap for the game's native beacon mesh once dumped
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    UStaticMeshComponent* BeaconMesh;

    // Pulsing point light to make the marker visible underwater
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOACopilot")
    UPointLightComponent* BeaconLight;

    // Override in Blueprint to register with the game's native map/compass system
    // TODO: find the map marker registration interface via UE4SS dumper
    UFUNCTION(BlueprintImplementableEvent, Category = "NOACopilot")
    void RegisterWithMap();

    UFUNCTION(BlueprintImplementableEvent, Category = "NOACopilot")
    void UnregisterFromMap();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
