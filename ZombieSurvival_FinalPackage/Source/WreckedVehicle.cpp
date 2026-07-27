// WreckedVehicle.cpp

#include "WreckedVehicle.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ItemData.h"

AWreckedVehicle::AWreckedVehicle()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    WreckMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WreckMesh"));
    RootComponent = WreckMesh;

    InteractionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRadius"));
    InteractionRadius->SetupAttachment(RootComponent);
    InteractionRadius->SetSphereRadius(300.0f);
    InteractionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // Domyślne sloty dla Buggy
    FVehiclePartSlot EngineSlot;
    EngineSlot.SlotName = FName("Engine");
    RequiredParts.Add(EngineSlot);

    FVehiclePartSlot WheelSlot;
    WheelSlot.SlotName = FName("Wheels");
    RequiredParts.Add(WheelSlot);

    FVehiclePartSlot BatterySlot;
    BatterySlot.SlotName = FName("Battery");
    RequiredParts.Add(BatterySlot);
}

void AWreckedVehicle::BeginPlay()
{
    Super::BeginPlay();
}

bool AWreckedVehicle::InstallPart(UItemData* PartItem, AActor* Installer)
{
    if (!PartItem || bIsFullyAssembled) return false;

    for (FVehiclePartSlot& Slot : RequiredParts)
    {
        if (!Slot.bIsOccupied && Slot.SlotName == PartItem->ItemName)
        {
            Slot.bIsOccupied = true;
            Slot.Condition = PartItem->Condition;
            CheckAssemblyComplete();
            return true;
        }
    }
    return false;
}

bool AWreckedVehicle::CanInstallPart(UItemData* PartItem) const
{
    if (!PartItem || bIsFullyAssembled) return false;
    return IsPartRequired(PartItem->ItemName);
}

bool AWreckedVehicle::IsPartRequired(FName PartName) const
{
    for (const FVehiclePartSlot& Slot : RequiredParts)
    {
        if (!Slot.bIsOccupied && Slot.SlotName == PartName)
        {
            return true;
        }
    }
    return false;
}

void AWreckedVehicle::CheckAssemblyComplete()
{
    int32 OccupiedCount = 0;
    for (const FVehiclePartSlot& Slot : RequiredParts)
    {
        if (Slot.bIsOccupied) OccupiedCount++;
    }

    AssemblyProgress = (float)OccupiedCount / (float)RequiredParts.Num() * 100.0f;
    OnRep_AssemblyProgress();

    if (OccupiedCount == RequiredParts.Num())
    {
        bIsFullyAssembled = true;
        FinishAssembly();
    }
}

void AWreckedVehicle::FinishAssembly()
{
    if (!AssembledVehicleClass) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AAssembledVehicle* NewVehicle = GetWorld()->SpawnActor<AAssembledVehicle>(
        AssembledVehicleClass,
        GetActorLocation(),
        GetActorRotation(),
        Params
    );

    if (NewVehicle)
    {
        // Przenieś condition części do pojazdu
        Destroy();
    }
}

float AWreckedVehicle::GetAssemblyPercent() const
{
    return AssemblyProgress;
}

void AWreckedVehicle::OnRep_AssemblyProgress()
{
    // Update UI progress bar
}

void AWreckedVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWreckedVehicle, AssemblyProgress);
    DOREPLIFETIME(AWreckedVehicle, bIsFullyAssembled);
}
