#include "NOACopilotMarker.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"

ANOACopilotMarker::ANOACopilotMarker()
{
    PrimaryActorTick.bCanEverTick = false;

    BeaconMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeaconMesh"));
    SetRootComponent(BeaconMesh);
    // TODO: assign the game's native beacon static mesh in the Blueprint subclass
    // (find the mesh asset path via Content Browser after cloning the template project)

    BeaconLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeaconLight"));
    BeaconLight->SetupAttachment(BeaconMesh);
    BeaconLight->SetIntensity(2000.f);
    BeaconLight->SetAttenuationRadius(800.f);
    // Light colour set per-category in Blueprint (e.g. orange for blackbox, cyan for scannable)
}

void ANOACopilotMarker::BeginPlay()
{
    Super::BeginPlay();
    RegisterWithMap();
}

void ANOACopilotMarker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterFromMap();
    Super::EndPlay(EndPlayReason);
}
